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
	if (!findRequestedLocation(_request.getHeader("Request-URI")))
	{
		setCustomErrorResponse(NOT_FOUND, getCustomErrorPath(NOT_FOUND));
		return;
	}

	// std::cerr << YELLOW << "request:\n" << _request << RESET;
	// std::cout << GREEN << "location: " << _location << RESET <<  std::endl;

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
	std::string body;
	std::string new_path;
	std::string last_modified = std::string();
	std::string location = std::string();
	std::string date = utils::formatDate(utils::get_timestamp(""));

	if (custom_error_path.empty())
	{
		body = generateHtmlErrorPage(status_code);
	}
	else if (isExternalRedirect(custom_error_path) 
		|| status_code == MOVED_PERMANENTLY 
		|| status_code == MOVED_TEMPORARY)
	{
		status_code = (status_code == MOVED_TEMPORARY || status_code == MOVED_PERMANENTLY) ? status_code : MOVED_TEMPORARY;
		location = custom_error_path;
	}
	else
	{
		if (!findRequestedLocation(custom_error_path))
		{
			setCustomErrorResponse(NOT_FOUND, "");
			return;
		}
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
			body = generateHtmlDirectoryListing(new_path);
		}
		else
		{
			body = generateHtmlErrorPage(status_code);
		}
		last_modified = utils::formatDate(utils::get_timestamp(new_path));
	}

	_response.setStatusCode(status_code);
	_response.setHeader("Date", date);
	_response.setHeader("Server", _server.server_name);

	if (!last_modified.empty())
		_response.setHeader("Last-Modified", last_modified);
	if (!body.empty())
		_response.setHeader("Content-Length", utils::itos(body.length()));
	if (!location.empty())
		_response.setHeader("Location", location);
	if (!body.empty())
	{
		_response.setBody(body);
		_response.setHeader("Content-Type", utils::get_MIME_type(body));
	}
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
		path = er_pages[status_code];
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
	return utils::is_str_in_vector(_request.getHeader("Request-Method"), _location.getAllowedMethods());
}

bool		RequestHandler::isImplementedMethod( void ) const
{
	return utils::is_str_in_vector(utils::str_toupper(_request.getHeader("Request-Method")), _server.getImplementedMethods());
}

bool		RequestHandler::isRedirection( void ) const
{
	return !_location.getRedirect().empty();
}

void	RequestHandler::setRedirectResponse( void )
{
	std::string error_path;
	std::string body;
	std::map<int, std::string>::const_iterator reds = _location.getRedirect().begin();
	int st_code = reds->first;

	_response.setStatusCode(st_code);
	_response.setHeader("Date", utils::formatDate(utils::get_timestamp("")));
	if (st_code == MOVED_PERMANENTLY || st_code == MOVED_TEMPORARY)
	{
		_response.setHeader("Location", reds->second);
	}
	else if (reds->second.empty() && !(error_path = getCustomErrorPath(reds->first)).empty())
	{
		body = utils::load_file_content(error_path);
		_response.setBody(body);
		_response.setHeader("Content-Length", utils::to_string(body.size()));
		_response.setHeader("Content-Type", utils::get_MIME_type(error_path));
	}
	else
	{
		_response.setBody(reds->second);
		_response.setHeader("Content-Length", utils::to_string(reds->second.size()));
		_response.setHeader("Content-Type", "text/plain");
	}

	_response.setHeader("Server", _server.server_name);
}

void		RequestHandler::handleStaticRequest( void )
{
	_request.setFullPath(determineFilePath(_request.getHeader("Request-URI")));

	if (std::strcmp(_request.getHeader("Request-Method").c_str(), "GET") == 0 || \
	std::strcmp(_request.getHeader("Request-Method").c_str(), "HEAD") == 0)
	{
		if (utils::is_regular_file(_request.getFullPath()))
		{
			_response.setStaticPageResponse(OK, _request.getFullPath());
		}
		else if (utils::is_directory(_request.getFullPath()))
		{
			handleGetDirectoryResponse();
		}
		else
		{
			setCustomErrorResponse(NOT_FOUND, getCustomErrorPath(NOT_FOUND));
		}
	}
	else if (std::strcmp(_request.getHeader("Request-Method").c_str(), "POST") == 0)
	{
		handlePOST();
	}
	else if (std::strcmp(_request.getHeader("Request-Method").c_str(), "DELETE") == 0)
	{
		handleDELETE();
	}
}

/** ------------------------ Methods related cgi request method ------------------------- */

bool	RequestHandler::isCGIRequest( void ) const
{
	std::string path_info = utils::extract_path_info(_request.getHeader("Request-URI"));
	std::string ext = utils::get_file_extension(_request.getHeader("Request-URI"));

	if (!utils::is_str_in_vector(ext, _location.getCGIExtension()))
	{
		return false;
	}
	_request.setHeader("Path-Info", path_info);
	return true;
}

void	RequestHandler::handleCgiRequest( void )
{
	CGI cgi;

	_request.setFullPath(determineFilePath(_request.getHeader("Request-URI")));

	if (!utils::has_executable_permissions(_request.getFullPath()))
	{
		setCustomErrorResponse(FORBIDDEN, getCustomErrorPath(FORBIDDEN));
		return;
	}

	cgi.setExecutable(_request.getFullPath());
	cgi.setUploadDir(searchingUploadPath());
	cgi.setEnvironment(_request);
	std::string data = cgi.executeCGI(_request);
	// std::cerr << GREEN << data << RESET << std::endl;
	handleCgiResponse(data);
}

void	RequestHandler::handleCgiResponse(const std::string& data)
{
	std::istringstream	iss(data);
	std::string			line;
	std::string			body;
	std::string			con_len;
	std::vector<std::pair<std::string, std::string> >	headers;

	while (std::getline(iss, line) && line.length() != 0 )
	{
		std::cerr << line << std::endl;
		utils::parse_header_field(line, headers);
	}

	std::ostringstream oss;
	oss << iss.rdbuf();
	body = oss.str();

	if (utils::get_value("Content-Type", headers).empty())
	{
		setCustomErrorResponse(INTERNAL_SERVER_ERROR, getCustomErrorPath(INTERNAL_SERVER_ERROR));
		return;
	}

	con_len = utils::get_value("content_length", headers);
	if (con_len.empty())
	{
		con_len = utils::to_string(body.length());
	}

	if (!utils::get_value("Status", headers).empty())
	{
		int st = utils::stoi(utils::get_value("Status", headers));
		_response.setStatusCode(st);
	}
	else
		_response.setStatusCode(200);

	_response.setBody(body);
	_response.setHeader("Date", utils::formatDate(utils::get_timestamp("")));
	_response.setHeader("Server", _server.server_name);
	_response.setHeader("Content-Type", utils::get_value("Content-Type", headers));
	_response.setHeader("Content-Length", con_len);
	if (!utils::get_value("Location", headers).empty())
	{
		_response.setStatusCode(302);
		_response.setHeader("Location", utils::get_value("Location", headers));
	}

	std::vector<std::pair<std::string, std::string> >::const_iterator it = headers.begin();
	std::vector<std::string> cookies_val;
	for (; it != headers.end(); ++it)
	{
		if (it->first == "Set-Cookie" && !utils::is_str_in_vector(it->first, cookies_val))
		{
			cookies_val.push_back(it->second);
			_response.setHeader("Set-Cookie", it->second);
		}
	}
}

/** ------------------------ Methods related GET request method ------------------------- */

void	RequestHandler::handleGetDirectoryResponse( void )
{
	std::string	new_path;

	if (!utils::ends_with(_request.getFullPath(), "/"))
	{
		setCustomErrorResponse(MOVED_PERMANENTLY, "");
		_response.setHeader("Location", _request.getHeader("Request-URI") + "/");
		return;
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
	std::string	body = generateHtmlDirectoryListing(_request.getFullPath());
	unsigned long long con_len = body.length();

	_response.setStatusCode(status_code);
	_response.setBody(body);
	_response.setHeader("Date", utils::formatDate(utils::get_timestamp("")));
	_response.setHeader("Server", _server.server_name);
	_response.setHeader("Content-Type", utils::get_MIME_type(body));
	_response.setHeader("Content-Length", utils::to_string(con_len));
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

std::string generateRawDataFilename()
{
	std::time_t now = std::time(NULL);
	std::tm *localTime = std::localtime(&now);

	char buf[20];

	std::strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", localTime);

    return "raw_data_" + std::string(buf);
}

void		RequestHandler::handlePOST( void )
{
	std::string uploadDir = searchingUploadPath();
	if (!utils::ends_with(uploadDir, "/"))
	{
		uploadDir = uploadDir + "/";
	}

	std::string filename = generateRawDataFilename();
	std::string body = _request.getBody();
	size_t max_size = (_location.getClientMaxBody() != 0) ? _location.getClientMaxBody() : _server.getClientMaxBody();
	if (body.size() > max_size)
	{
		setCustomErrorResponse(REQUEST_ENTITY_TOO_LARGE, getCustomErrorPath(REQUEST_ENTITY_TOO_LARGE));
		return;
	}
	std::string filepath = uploadDir + filename;

	std::ofstream outfile(filepath.c_str(), std::ios::binary);
	if (!outfile.is_open())
	{
		std::cerr	<< "Failed to open file: " << filepath
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

	outfile.write(body.c_str(), body.length());
	if (outfile.fail())
	{
		std::cerr << "Error writing to file: " << filepath << std::endl;
		setCustomErrorResponse(INTERNAL_SERVER_ERROR, getCustomErrorPath(INTERNAL_SERVER_ERROR));
		outfile.close();
		return;
	}
	outfile.close();
	_response.setStatusCode(CREATED);
}

std::string		RequestHandler::searchingUploadPath( void )
{
	std::string	uploadDir = "";
	std::string fullPath = "";

	if (_location.getUploadDir().empty() && !_server.getUploadDir().empty())
	{
		uploadDir = _server.getUploadDir();
		findRequestedLocation(uploadDir); //added
		fullPath = determineFilePath(uploadDir);
	}
	else if (!_location.getUploadDir().empty())
	{
		uploadDir = _location.getUploadDir();
		findRequestedLocation(uploadDir); //added
		fullPath = determineFilePath(uploadDir);
	}

	// std::cerr << YELLOW << _location << RESET << std::endl;
	// std::cerr << GREEN << "_server.getUploadDir() " << _server.getUploadDir() << "\n" <<
	// " _location.getUploadDir() " << _location.getUploadDir() << " \fullPath  "<<
	// fullPath << RESET << std::endl; //rm

	return fullPath;
}

/** ------------------------ Methods related DELETE request method ------------------------- */

void		RequestHandler::handleDELETE( void )
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

/** @return status code */
int			RequestHandler::handleDeleteDirectoryResponse( void )
{
	if (!utils::ends_with(_request.getFullPath(), "/"))
	{
		return CONFLICT;
	}
	if (!utils::has_user_permissions(_request.getFullPath()))
	{
		return FORBIDDEN;
	}
	return FORBIDDEN;
	/*
	int rc = 0;
	if ((rc = remove_directory_recursively(_request.getFullPath())) != NO_CONTENT)
	{
		return rc;
	}
	if (rmdir(_request.getFullPath().c_str()) == 0)
	{
		return NO_CONTENT;
	}

	return INTERNAL_SERVER_ERROR;
	*/
}

/** @return status code */
int			RequestHandler::remove_file(const std::string& path)
{
	std::string dir = utils::substr_before_rdel(path, "/");
	if (!utils::has_user_permissions(dir))
	{
		return FORBIDDEN;
	}
	if (std::remove(path.c_str()) == 0)
	{
		return NO_CONTENT;
	}
	return INTERNAL_SERVER_ERROR;
}

/** @return status code */
int			RequestHandler::remove_directory_recursively(const std::string& path)
{
	std::vector<std::string> cont;
	int rc = 0;
	if (utils::get_dir_entries(path, cont) != 0)
	{
		return FORBIDDEN;
	}

	std::vector<std::string>::iterator it = cont.begin();
	for (; it != cont.end(); ++it)
	{
		if ((*it).compare("..") == 0)
			continue;
		if (utils::is_regular_file(path + (*it)))
		{
			if (std::remove((path + (*it)).c_str()) != 0)
				return INTERNAL_SERVER_ERROR;
		}
		else
		{
			if ((rc = remove_directory_recursively(path + (*it))) != NO_CONTENT)
				return rc;
			if (rmdir((path + (*it)).c_str()) != 0)
				return INTERNAL_SERVER_ERROR;
		}
	}
	return NO_CONTENT;
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
	return (!utils::starts_with(path, "/")
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
std::string RequestHandler::normalizePath(const std::string& path)
{
	std::vector<std::string> stack;
	std::stringstream ss(path);
	std::string segment;

	while (std::getline(ss, segment, '/'))
	{
		if (segment == "..")
		{
			if (!stack.empty())
				stack.pop_back();
		}
		else if (!segment.empty() && segment != ".")
		{
			stack.push_back(segment);
		}
	}

	std::string normalizedPath = "/";
	for (size_t i = 0; i < stack.size(); ++i)
	{
		normalizedPath += stack[i];
		if (i < stack.size() - 1)
			normalizedPath += "/";
	}
	return normalizedPath;
}

std::string			RequestHandler::canonicalizePath(const std::string& path)
{
	/* https://datatracker.ietf.org/doc/html/rfc3986#section-3.3 */
	if (path.empty())
		return "/";
	std::string new_path = path;

	new_path = decodeURI(new_path);
	new_path = normalizePath(new_path);
	return new_path;
}

std::string	RequestHandler::generateHtmlDirectoryListing( const std::string& path)
{
	std::stringstream html;

	html << "<!DOCTYPE html>\n"
			"<html lang=\"en\">\n"
			"<head>\n"
			"    <meta charset=\"UTF-8\">\n"
			"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
			"    <title>Directory Listing</title>\n"
			"    <style>\n"
			"        body {\n"
			"            font-size: 20px;\n"
			"            font-family: Arial, sans-serif;\n"
			"        }\n"
			"        ul {\n"
			"            list-style-type: square;\n"
			"        }\n"
			"    </style>"
			"</head>\n"
			"<body>\n"
			"<h1>Index of "
			<< _location.getPath()
			<< "</h1>"
			"    <ul>\n";

	std::vector<std::string> dir_content;

	if (utils::get_dir_entries(path, dir_content) == 0)
	{
		for (size_t i = 0; i < dir_content.size(); i++)
		{
			std::string	d_name = dir_content[i];
			if (d_name != ".")
			{
				html << "        <li><a href=\"" << d_name << "\">" << d_name << "</a></li>\n";
			}
		}
	}
	else
	{
		html << "       <p>Error: Could not open directory.</p>\n";
	}

	html << "    </ul>\n"
			"</body\n>"
			"</html>\n";

	return html.str();
}

std::string	RequestHandler::generateHtmlErrorPage( int status_code )
{
	std::stringstream	html;

	html << "<!DOCTYPE html>\n"
			"<html lang=\"en\">\n"
			"<head>\n"
			"    <meta charset=\"UTF-8\">\n"
			"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
			"    <title>"
			<< status_code
			<< " "
			<< utils::get_reason_phrase(status_code)
			<< "</title>\n"
			"    <style>\n"
			"        body {\n"
			"            display: flex;\n"
			"            justify-content: center;\n"
			"            align-items: center;\n"
			"            height: 100vh;\n"
			"            margin: 0;\n"
			"            font-family: Arial, sans-serif;\n"
			"        }\n"
			"        .error-container {\n"
			"            text-align: center;\n"
			"            background: #fff;\n"
			"            padding: 20px;\n"
			"        }\n"
			"        .error-container h1 {\n"
			"            font-size: 3em;\n"
			"            margin: 0 0 10px;\n"
			"        }\n"
			"        .error-container p {\n"
			"            font-size: 1.2em;\n"
			"            margin: 0 0 15px;\n"
			"        }\n"
			"    </style>"
			"</head>\n"
			"<body>\n"
			"    <div class=\"error-container\">"
			"        <h1>"
			<< status_code
			<< " "
			<< utils::get_reason_phrase(status_code)
			<<"</h1>\n"
			"        <p>"
			<< utils::get_status_message(status_code)
			<< "</p>\n"
			"</body>\n"
			"</html>\n";

	return html.str();
}
