#include "../includes/CGI.hpp"
#include "../includes/Server.hpp"
#include "../includes/Request.hpp"
#include "../includes/Location.hpp"
#include "../includes/Response.hpp"
#include "../includes/RequestHandler.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

RequestHandler::RequestHandler(const Server& server, Request& request, Response& response) : 
	_server(server), 
	_request(request), 
	_response(response)
{}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

RequestHandler::~RequestHandler() {}

/*
** --------------------------------- METHODS ----------------------------------
*/

void		RequestHandler::processRequest()
{
	if (!findRequestedLocation(_request.getURI()))
	{
		setCustomErrorResponse(NOT_FOUND, getCustomErrorPath(NOT_FOUND));
		return;
	}

	_request.setFullPath(determineFilePath(_request.getURI()));
	
	// std::cout << "_request.getURI(): " << _request.getURI() << std::endl;
	// std::cout << "location: " << _location << std::endl;
	// std::cerr << "_request " << _request << std::endl;

	if (!isImplementedMethod())
	{
		setCustomErrorResponse(NOT_IMPLEMENTED, getCustomErrorPath(NOT_IMPLEMENTED));
		_response.setHeader("Allow", utils::vector_tostr(_location.getAllowedMethods()));
		return;
	}

	if (!isMethodAllowed())
	{
		setCustomErrorResponse(METHOD_NOT_ALLOWED, getCustomErrorPath(METHOD_NOT_ALLOWED));
		_response.setHeader("Allow", utils::vector_tostr(_location.getAllowedMethods()));
		return;
	}

	if (isRedirection())
	{
		setRedirectResponse();
		return;
	}

	if (isCGIRequest())
	{
		handleCgiRequest();
	}
	else
	{
		handleStaticRequest();
	}
}

void	RequestHandler::setCustomErrorResponse(int status_code, const std::string& custom_error_path)
{
	// std::string path = getCustomErrorPath(status_code);
	std::string body;
	std::string new_path;
	std::string last_modified = std::string();
	std::string location = std::string();
	std::string date = utils::formatDate(utils::get_timestamp(""));

	if (custom_error_path.empty())
	{
		body = utils::generate_html_error_page(status_code);
	}
	else if (isExternalRedirect(custom_error_path))
	{
		status_code = MOVED_TEMPORARY;
		location = custom_error_path;
	}
	else
	{
		new_path = determineFilePath(custom_error_path);
		if (utils::is_directory(new_path))
		{
			new_path = appendIndexFile(new_path);
		}
		if (utils::is_regular_file(new_path))
		{
			body = utils::load_file_content(new_path);
		}
		else if (_location.getAutoindex())
		{
			body = utils::generate_html_directory_listing(new_path);
		}
		else
		{
			body = utils::generate_html_error_page(status_code);
		}
		last_modified = utils::formatDate(utils::get_timestamp(new_path));
	}

	_response.setStatusCode(status_code);
	_response.setHeader("Date", date);
	_response.setHeader("Server", _server.server_name);
	_response.setHeader("Content-Type", utils::get_MIME_type(body));
	if (!last_modified.empty()) _response.setHeader("Last-Modified", last_modified);
	if (!body.empty()) _response.setHeader("Content-Length", utils::itos(body.length()));
	if (!location.empty()) _response.setHeader("Location", location);
	if (!body.empty()) _response.setBody(body);
}

/** Check for custom error pages if any, othewise generate default html error page */
std::string		RequestHandler::getCustomErrorPath(int status_code)
{
	std::string					path = std::string();
	std::map<int, std::string>	er_pages;

	er_pages = _location.getErrorPages();
	if (er_pages.size() == 0 && !_server.getErrorPages().empty())
	{
		er_pages = _server.getErrorPages();
	}

	if (er_pages.size() > 0 && er_pages.find(status_code) != er_pages.end())
	{
		// path = er_pages[status_code];
		if (findRequestedLocation(er_pages[status_code]))
		{
			path = determineFilePath(er_pages[status_code]);
		}
	}
	return path;
}

/** Formates the relative requested URI based on root and alias directive from server and location block */
std::string			RequestHandler::determineFilePath(const std::string& requested_path)
{
	std::string path = requested_path;
	std::string	root = "";

	if (!_location.getRoot().empty())
	{
		root = _location.getRoot();
	}
	else if (!_server.getRoot().empty())
	{
		root = _server.getRoot();
	}

	if (!_location.getAlias().empty())
	{
		std::string	loc_path = _location.getPath();
		if (utils::starts_with(path, loc_path))
		{
			root = _location.getAlias();
			path.erase(0, loc_path.length());
		}
	}

	std::string full_path = root + path;
	return full_path;
}

bool		RequestHandler::isMethodAllowed( void ) const
{
	return utils::is_str_in_vector(_request.getMethod(), _location.getAllowedMethods());
}

bool		RequestHandler::isImplementedMethod( void ) const
{
	return utils::is_str_in_vector(utils::str_toupper(_request.getMethod()), _server.getImplementedMethods());
}

bool		RequestHandler::isRedirection( void ) const
{
	return !_location.getRedirect().empty();
}

void	RequestHandler::setRedirectResponse( void )
{
	std::map<int, std::string>::const_iterator reds = _location.getRedirect().begin();

	_response.setStatusCode(MOVED_PERMANENTLY);
	_response.setHeader("Date", utils::formatDate(utils::get_timestamp("")));
	_response.setHeader("Location", reds->second);
	_response.setHeader("Server", _server.server_name);
}

bool	RequestHandler::isCGIRequest( void ) const
{
	std::string ext = utils::get_file_extension(_request.getURI());

	if (!utils::is_str_in_vector(ext, _location.getCGIExtension()))
	{
		return false;
	}
	return true;
}

void	RequestHandler::handleCgiRequest( void )
{
	CGI cgi;

	cgi.setEnvironment(_request);
	cgi.setUploadDir(searchingUploadDir());
	cgi.setExecutable(_request.getFullPath());
	std::string body = cgi.executeCGI(_request);

	_response.setBody(body);
	unsigned long long con_len = body.length();
	_response.setStatusCode(200);
	_response.setHeader("Date", utils::formatDate(utils::get_timestamp("")));
	_response.setHeader("Server", _server.server_name);
	_response.setHeader("Content-Type", utils::get_MIME_type(body));
	_response.setHeader("Content-Length", utils::ulltos(con_len));
}

void		RequestHandler::handleStaticRequest( void )
{
	if (std::strcmp(_request.getMethod().c_str(), "GET") == 0 || \
		std::strcmp(_request.getMethod().c_str(), "HEAD") == 0)
	{
		if (utils::is_regular_file(_request.getFullPath()))
		{
			_response.setStaticPageResponse(OK, _request.getFullPath());
		}
		else
		{
			handleGetDirectoryResponse();
		}
	}
	else if (std::strcmp(_request.getMethod().c_str(), "POST") == 0)
	{
		handlePOST();
	}
	else if (std::strcmp(_request.getMethod().c_str(), "DELETE") == 0)
	{
		handleDELETE();
	}
}
/** ------------------------ Methods related GET request method ------------------------- */

void		RequestHandler::handleGET( void )
{
	if (utils::is_regular_file(_request.getFullPath()))
	{
		_response.setStaticPageResponse(OK, _request.getFullPath());
	}
	else
	{
		handleGetDirectoryResponse();
	}
}

void	RequestHandler::handleGetDirectoryResponse( void )
{
	std::string	new_path;

	if (!utils::is_directory(_request.getFullPath()))
	{
		setCustomErrorResponse(NOT_FOUND, getCustomErrorPath(NOT_FOUND));
	}
	else if (!utils::ends_with(_request.getFullPath(), "/"))
	{
		setCustomErrorResponse(MOVED_PERMANENTLY, "");
		_response.setHeader("Location", _request.getURI() + "/");
	}

	new_path = appendIndexFile(_request.getFullPath());
	if (new_path.compare(_request.getFullPath().c_str()) != 0)
	{
		_response.setStaticPageResponse(OK, new_path);
	}
	else if (!_location.getAutoindex())
	{
		setCustomErrorResponse(FORBIDDEN_DIR, getCustomErrorPath(FORBIDDEN_DIR));
	}
	else
	{
		setDirectoryListingResponse(OK);
	}
}

void	RequestHandler::setDirectoryListingResponse(int status_code)
{
	std::string	body = utils::generate_html_directory_listing(_request.getFullPath());
	unsigned long long con_len = body.length();

/* 	if (con_len <= 0)
	{
		status_code = NO_CONTENT;
	} */

	_response.setStatusCode(status_code);
	_response.setBody(body);
	_response.setHeader("Date", utils::formatDate(utils::get_timestamp("")));
	_response.setHeader("Server", _server.server_name);
	_response.setHeader("Content-Type", utils::get_MIME_type(body));
	_response.setHeader("Content-Length", utils::ulltos(con_len));
}

std::string		RequestHandler::appendIndexFile( const std::string& path )
{
	std::vector<std::string> dir_content;
	std::vector<std::string> idxs = _location.getIndexes();
	std::string				new_path = path;

	if (idxs.size() == 0)
	{
		idxs = _server.getIndexes();
	}

	if (utils::get_dir_entries(path, dir_content) != 0)
	{
		return path;
	}

	for (size_t i = 0; i < idxs.size(); i++)
	{
		if (utils::is_str_in_vector(idxs[i], dir_content))
		{
			new_path += idxs[i];
			return new_path;
		}
	}

	return path;
}

/** ------------------------ Methods related POST request method ------------------------- */
/**  TODO: if content_length more if max_body-size */ 
void		RequestHandler::handlePOST( void )
{
	std::string uploadDir = searchingUploadDir();

	if (utils::substr_after_rdel(_request.getFullPath(), ".").empty())
	{
		setCustomErrorResponse(FORBIDDEN, getCustomErrorPath(FORBIDDEN));
		return;
	}
	std::string uploadFile = utils::substr_after_rdel(_request.getFullPath(), "/");
	std::ofstream file((uploadDir + uploadFile).c_str()); // "/" ??
	if (!file.is_open())
	{
		std::cerr	<< "Failed to open file: " << _request.getFullPath()
					<< " (" << strerror(errno) << ")" << std::endl;

		if (errno == EACCES || errno == EROFS || errno == ENOENT)
		{
			setCustomErrorResponse(FORBIDDEN, getCustomErrorPath(FORBIDDEN));
		}
		else
		{
			setCustomErrorResponse(INTERNAL_SERVER_ERROR, getCustomErrorPath(INTERNAL_SERVER_ERROR));
		}

		return;
	}

	file << _request.getBody();
	if (file.fail())
	{
		std::cerr << "Error writing to file: " << uploadDir + uploadFile << std::endl;
		setCustomErrorResponse(INTERNAL_SERVER_ERROR, getCustomErrorPath(INTERNAL_SERVER_ERROR));
		file.close();
		return;
	}
	file.close();
	_response.setStatusCode(CREATED);
}

std::string		RequestHandler::searchingUploadDir( void )
{
	std::string	uploadDir = "";
	std::string fullPath = "";

	if (_location.getUploadDir().empty() && !_server.getUploadDir().empty())
	{
		uploadDir = _location.getUploadDir();
		fullPath = determineFilePath(uploadDir);
	}
	else if (!_location.getUploadDir().empty())
	{
		uploadDir = _location.getUploadDir();
		fullPath = determineFilePath(uploadDir);
	}

	return fullPath;
}

/** ------------------------ Methods related DELETE request method ------------------------- */

void		RequestHandler::handleDELETE( void )
{
	if (!utils::has_write_permission(_request.getFullPath()))
	{
		setCustomErrorResponse(FORBIDDEN, getCustomErrorPath(FORBIDDEN));
	}
	else
	{
		if (utils::is_regular_file(_request.getFullPath()))
		{
			int status_code = remove_file(_request.getFullPath());
			if (utils::is_client_error(status_code) || utils::is_server_error(status_code))
			{
				setCustomErrorResponse(status_code, getCustomErrorPath(status_code));
				return;
			}
			else
			{
				_response.setStatusCode(status_code);
			}
		}
		else if (utils::is_directory(_request.getFullPath()))
		{
			int status_code = handleDeleteDirectoryResponse();
			if (utils::is_client_error(status_code) || utils::is_server_error(status_code))
			{
				setCustomErrorResponse(status_code, getCustomErrorPath(status_code));
				return;
			}
			else
			{
				_response.setStatusCode(status_code);
			}
		}
		else
		{
			setCustomErrorResponse(NOT_FOUND, getCustomErrorPath(NOT_FOUND));
		}
	}
}

/** @return status code */
int			RequestHandler::handleDeleteDirectoryResponse( void )
{
	if (!utils::ends_with(_request.getFullPath(), "/"))
	{
		return CONFLICT;
	}
	else if (!utils::has_write_permission(_request.getFullPath()))
	{
		return FORBIDDEN;
	}
	else
	{
		return remove_directory(_request.getFullPath());
	}
}

/** @return status code */
int			RequestHandler::remove_file(const std::string& path)
{
	if (std::remove(path.c_str()) == 0)
	{
		return NO_CONTENT;
	}
	return INTERNAL_SERVER_ERROR;
}

/** @return status code */
int			RequestHandler::remove_directory(const std::string& path)
{
	std::vector<std::string> cont;

	if (utils::get_dir_entries(path, cont) == 0)
	{
		return CONFLICT;
	}
	if (rmdir(path.c_str()) == 0)
	{
		return NO_CONTENT;
	}

	return INTERNAL_SERVER_ERROR;
}

/* ------------------------ Methods related to Location  ------------------------- */

/** Based on a requested path searching for the location */
bool		RequestHandler::findRequestedLocation(const std::string& path)
{
	bool location_found;

	location_found = searchingExtensionMatchLocation(path);
	if (!location_found)
	{
		location_found = searchingPrefixMatchLocation(path);
	}

	return location_found;
}

bool		RequestHandler::searchingExtensionMatchLocation(const std::string& requested_path)
{
	size_t pos = 0;
	std::vector<Location>::const_iterator it = _server.getLocations().begin();

	for (; it < _server.getLocations().end(); it++)
	{
		std::string loc_path = it->getPath();
		if ((pos = loc_path.find('*')) == std::string::npos)
			continue;
		std::string ext = loc_path.substr(pos + 1);
		if (!utils::ends_with(requested_path, ext))
			continue;
		_location = *it;
		return true;
	}
	return false;
}


/**
 * Normalize a URI by resolving its root and determining the longest prefix-based match from location blocks.
 *
 * @param requested_path - The requested URI path
 * @param location - The relevant Location object will be stored here.
 * @return true - if the location found
 */
bool		RequestHandler::searchingPrefixMatchLocation(const std::string& requested_path)
{
	std::string	best_match = std::string();
	bool	location_found = false;

	std::vector<Location>::const_iterator it = _server.getLocations().begin();

	for (; it < _server.getLocations().end(); ++it)
	{
		std::string loc_path = it->getPath();
		if (requested_path.compare(0, loc_path.length(), loc_path) == 0)
		{
			if (loc_path.length() > best_match.length())
			{
				best_match = loc_path;
				_location = *it;
				location_found = true;
			}
		}
	}
	return location_found;
}

bool		RequestHandler::isExternalRedirect(const std::string& path)
{
	return (utils::is_regular_file(path)
			|| utils::starts_with(path, "http://")
			|| utils::starts_with(path, "https://"));
}

std::string			RequestHandler::decodeURI(const std::string& path)
{
	std::ostringstream decoded;
	for (size_t i = 0; i < path.length(); ++i)
	{
		if (path[i] == '%' && i + 2 < path.length() && isxdigit(path[i + 1]) && isxdigit(path[i + 2]))
		{
			int value;
			std::istringstream hexStream(path.substr(i + 1, 2));
			hexStream >> std::hex >> value;
			decoded << static_cast<char>(value);
			i += 2;
		}
		else
		{
			decoded << path[i];
		}
	}
	return decoded.str();
}


std::string			RequestHandler::canonicalizePath(const std::string& path)
{
	/* https://datatracker.ietf.org/doc/html/rfc3986#section-3.3 */
	if (path.empty())
		return "/";
	std::string new_path = path;

	new_path = decodeURI(new_path);
	return new_path;
}
