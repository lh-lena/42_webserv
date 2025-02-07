#include "../includes/CGI.hpp"
#include "../includes/Server.hpp"
#include "../includes/Request.hpp"
#include "../includes/Location.hpp"
#include "../includes/Response.hpp"
#include "../includes/RequestHandler.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

RequestHandler::RequestHandler(const Server& server, const Request& request, Response& response) : 
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
	if (!findRequestedLocation(_request.reqURI))
	{
		setErrorResponse(NOT_FOUND);
		return;
	}

	_path = determineFilePath(_request.reqURI);

	if (!isImplementedMethod())
	{
		_response.setHeader("Allow", utils::vector_tostr(_location.getAllowedMethods()));
		setErrorResponse(NOT_IMPLEMENTED);
		return;
	}

	if (!isMethodAllowed())
	{
		_response.setHeader("Allow", utils::vector_tostr(_location.getAllowedMethods()));
		setErrorResponse(METHOD_NOT_ALLOWED);
		return;
	}

	if (isRedirection())
	{
		setRedirectResponse();
		return;
	}

	if (isCGIRequest())
	{
		CGI cgi;

		cgi.handleRequest(_request); /** TODO: to add a response */
	}
	else
	{
		handleStaticRequest();
	}
}

void	RequestHandler::setErrorResponse(int status_code)
{
	_response.setStatusCode(status_code);
	std::string path = getCustomErrorPage(status_code);
	std::string body;
	std::string last_modified;
	std::string date = utils::formatDate(utils::get_timestamp(""));

	if (!path.empty())
	{
		body = utils::load_file_content(path);
		last_modified = utils::formatDate(utils::get_timestamp(path));
	}
	else
	{
		body = utils::generate_html_error_page(status_code);
		last_modified = date;
	}
	_response.setBody(body);
	_response.setHeader("Date", date);
	_response.setHeader("Server", _server.server_name);
	_response.setHeader("Last-Modified", last_modified);
	_response.setHeader("Content-Type", utils::get_MIME_type(body));
	_response.setHeader("Content-Length", utils::itos(body.length()));
}

/** Check for custom error pages if any, othewise generate default html error page */
std::string		RequestHandler::getCustomErrorPage(int status_code)
{
	std::string					path = std::string();
	Location					loc;
	std::map<int, std::string>	er_pages;

	er_pages = _location.getErrorPages();
	if (er_pages.size() == 0 && !_server.getErrorPages().empty())
	{
		er_pages = _server.getErrorPages();
	}

	if (er_pages.size() > 0 && er_pages.find(status_code) != er_pages.end())
	{
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
	std::string	root = std::string();
	// std::cout << "location: " << *loc << std::endl;
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
		if (path.find(loc_path) == 0)
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
	return utils::is_str_in_vector(_request.method, _location.getAllowedMethods());
}

bool		RequestHandler::isImplementedMethod( void ) const
{
	return utils::is_str_in_vector(utils::str_toupper(_request.method), _server.getImplementedMethods());
}

bool		RequestHandler::isRedirection( void ) const
{
	return !_location.getRedirect().empty();
}

void	RequestHandler::setRedirectResponse( void )
{
	std::map<int, std::string>::const_iterator reds = _location.getRedirect().begin();

	_response.setStatusCode(reds->first);
	_response.setHeader("Location", reds->second);
	_response.setHeader("Date", utils::formatDate(utils::get_timestamp("")));
	_response.setHeader("Server", _server.server_name);
}

bool	RequestHandler::isCGIRequest( void ) const
{
	std::string ext = utils::get_file_extension(_request.reqURI);

	if (!utils::is_str_in_vector(ext, _location.getCGIExtension()))
	{
		return false;
	}

	const std::map<std::string, std::string>& cgiInterpreters = _location.getCgiInterpreters();

	return cgiInterpreters.find(ext) != cgiInterpreters.end();
}

void		RequestHandler::handleStaticRequest( void )
{
	// std::cout << "request.method " << request.method << std::endl;
	if (std::strcmp(_request.method.c_str(), "GET") == 0 || \
		std::strcmp(_request.method.c_str(), "HEAD") == 0)
	{
		handleGET();
	}
	else if (std::strcmp(_request.method.c_str(), "POST") == 0)
	{
		handlePOST();
	}
	else if (std::strcmp(_request.method.c_str(), "DELETE") == 0)
	{
		handleDELETE();
	}
}
/** ------------------------ Methods related GET request method ------------------------- */

void		RequestHandler::handleGET( void )
{
	if (utils::is_regular_file(_path))
	{
		setGetResponse(OK);
	}
	else
	{
		handleGetDirectoryResponse();
	}
}

void	RequestHandler::setGetResponse(int status_code)
{
	std::string	body = utils::load_file_content(_path);
	unsigned long long con_len = body.length(); 

	if (con_len <= 0)
	{
		status_code = NO_CONTENT;
	}

	_response.setStatusCode(status_code);
	_response.setBody(body);
	_response.setHeader("Date", utils::formatDate(utils::get_timestamp("")));
	_response.setHeader("Server", _server.server_name);
	_response.setHeader("Last-Modified", utils::formatDate(utils::get_timestamp(_path)));
	_response.setHeader("Content-Type", utils::get_MIME_type(_path));
	_response.setHeader("Content-Length", utils::ulltos(con_len));
}

void	RequestHandler::handleGetDirectoryResponse( void )
{
	if (!utils::is_directory(_path))
	{
		setErrorResponse(NOT_FOUND);
	}
	else if (!utils::ends_with(_path, "/"))
	{
		_response.setHeader("Location", _request.reqURI + "/");
		setErrorResponse(MOVED_PERMANENTLY);
	}
	else if (appendIndexFile())
	{
		setGetResponse(OK);
	}
	else if (!_location.getAutoindex())
	{
		setErrorResponse(FORBIDDEN_DIR);
	}
	else
	{
		setDirectoryListingResponse(OK);
	}
}


void	RequestHandler::setDirectoryListingResponse(int status_code)
{
	std::string	body = utils::generate_html_directory_listing(_path);
	unsigned long long con_len = body.length();

	if (con_len <= 0)
	{
		status_code = NO_CONTENT;
	}

	_response.setStatusCode(status_code);
	_response.setBody(body);
	_response.setHeader("Date", utils::formatDate(utils::get_timestamp("")));
	_response.setHeader("Server", _server.server_name);
	_response.setHeader("Content-Type", utils::get_MIME_type(body));
	_response.setHeader("Content-Length", utils::ulltos(con_len));
}

bool	RequestHandler::appendIndexFile( void )
{
	std::vector<std::string> dir_content;
	std::vector<std::string> idxs = _location.getIndexes();

	if (idxs.size() == 0)
	{
		idxs = _server.getIndexes();
	}

	if (utils::get_dir_entries(_path, dir_content) != 0)
	{
		return false;
	}

	for (size_t i = 0; i < idxs.size(); i++)
	{
		if (utils::is_str_in_vector(idxs[i], dir_content))
		{
			_path += idxs[i];
			return true;
		}
	}

	return false;
}

/** ------------------------ Methods related POST request method ------------------------- */

void		RequestHandler::handlePOST( void )
{
	std::string uploadDir = searchingUploadDir();

	if (utils::substr_after_rdel(_path, ".").empty())
	{
		setErrorResponse(FORBIDDEN);
		return;
	}
	std::string uploadFile = utils::substr_after_rdel(_path, "/");
	std::ofstream file((uploadDir + uploadFile).c_str()); // "/" ??
	if (!file.is_open())
	{
		std::cerr	<< "Failed to open file: " << _path
					<< " (" << strerror(errno) << ")" << std::endl;

		if (errno == EACCES || errno == EROFS || errno == ENOENT)
		{
			setErrorResponse(FORBIDDEN);
		}
		else
		{
			setErrorResponse(INTERNAL_SERVER_ERROR);
		}

		return;
	}

	file << _request.reqBody;
	if (file.fail())
	{
		std::cerr << "Error writing to file: " << uploadDir + uploadFile << std::endl;
		setErrorResponse(INTERNAL_SERVER_ERROR);
		file.close();
		return;
	}
	file.close();
	_response.setStatusCode(CREATED);
}

/** FIX: if specified upload_dir directive will append to root, otherwise ?what? */
std::string		RequestHandler::searchingUploadDir( void )
{
	if (_location.getUploadDir().empty())
	{
		return std::string();
	}

	std::string	uploadDir = _location.getUploadDir();
	std::string fullPath = determineFilePath(uploadDir);
	return fullPath;
}

/** ------------------------ Methods related DELETE request method ------------------------- */

void		RequestHandler::handleDELETE( void )
{
	if (!utils::has_write_permission(_path))
	{
		setErrorResponse(FORBIDDEN);
	}
	else
	{
		if (utils::is_regular_file(_path))
		{
			int status_code = remove_file(_path);
			if (utils::is_client_error(status_code) || utils::is_server_error(status_code))
			{
				setErrorResponse(status_code);
				return;
			}
			else
			{
				_response.setStatusCode(status_code);
			}
		}
		else if (utils::is_directory(_path))
		{
			int status_code = handleDeleteDirectoryResponse();
			if (utils::is_client_error(status_code) || utils::is_server_error(status_code))
			{
				setErrorResponse(status_code);
				return;
			}
			else
			{
				_response.setStatusCode(status_code);
			}
		}
		else
		{
			setErrorResponse(NOT_FOUND);
		}
	}
}

/** @return status code */
int			RequestHandler::handleDeleteDirectoryResponse( void )
{
	if (!utils::ends_with(_path, "/"))
	{
		return CONFLICT;
	}
	else if (!utils::has_write_permission(_path))
	{
		return FORBIDDEN;
	}
	else
	{
		return remove_directory(_path);
	}
}

/** @return status code */
int			RequestHandler::remove_file(const std::string& path)
{
	if (std::remove(path.c_str())  == 0)
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

/* ------------------------ Methods related Location searching ------------------------- */

/** Based on a requested path searching for the location */
bool		RequestHandler::findRequestedLocation(const std::string& path)
{
	bool location_found;

	location_found = searchingExtensionMatchLocation(path);
	if (!location_found)
	{
		location_found = searchingPrefixMatchLocation(path);
	}

	// std::cout << "location:" << _location;

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
		loc_path = loc_path.substr(0, pos);
		if (!utils::ends_with(requested_path, ext))
			continue;
		if (!loc_path.empty() && std::strncmp(loc_path.c_str(), requested_path.c_str(), loc_path.length()) != 0)
			continue;
		_location = *it;
		return true;
	}
	return false;
}

/**
 * Normalize a URI by resolving its root and determining the relevant prefix-based location blocks.
 *
 * @param requested_path - The requested URI path
 * @param location - The relevant Location object will be stored here.
 * @return true - if the location found
 */
bool		RequestHandler::searchingPrefixMatchLocation(const std::string& requested_path)
{
	std::string	searched_path = requested_path;
	std::string	rest = std::string();
	bool	location_found = false;
	size_t	pos = 0;

	while (!searched_path.empty())
	{
		std::vector<Location>::const_iterator it = _server.getLocations().begin();

		for (; it < _server.getLocations().end(); it++)
		{
			std::string loc_path = it->getPath();
			if (std::strcmp(searched_path.c_str(), loc_path.c_str()) == 0)
			{
				_location = *it;
				location_found =  true;
				break;
			}
		}
		pos = searched_path.rfind("/");
		if (pos != std::string::npos && utils::ends_with(searched_path, "/"))
		{
			rest = requested_path.substr(pos);
			searched_path = searched_path.erase(pos);
		}
		else if (pos != std::string::npos && searched_path[pos + 1])
		{
			rest = requested_path.substr(pos + 1);
			searched_path = searched_path.erase(pos + 1);
		}
		else
		{
			rest = requested_path;
			searched_path = "";
		}
		if (location_found)
			break;
	}
	return location_found;
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

	if (new_path[0] != '/')
		new_path = "/" + new_path;

	new_path = decodeURI(new_path);
	return new_path;
}
