#include "../includes/Server.hpp"
#include "../includes/Response.hpp"
#include "../includes/Request.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Server::Server()
	:
		_location_nbr(0),
		_client_max_body_size(1 * 1024 * 1024), //1M
		_worker_connections(1024),
		_port(8080), // ?
		_host("localhost")
{
}

Server::Server( const Server & src )
{
	*this = src;
}


/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Server::~Server()
{
}


/*
** --------------------------------- OVERLOAD ---------------------------------
*/

Server &				Server::operator=( Server const & rhs )
{
	if ( this != &rhs )
	{
		this->_location_nbr = rhs.getLocationNbr();
		this->_client_max_body_size = rhs.getClientMaxBody();
		this->_worker_connections = rhs.getWorkCont();
		this->_host = rhs.getHost();
		this->_port = rhs.getPort();
		this->_root = rhs.getRoot();
		this->_error_log = rhs.getErrorLog();
		this->_error_pages = rhs.getErrorPages();
		this->_indexes = rhs.getIndexes();
		this->_server_names  = rhs.getServerNames();
		this->_locations = rhs.getLocations();
	}
	return *this;
}

std::ostream&			operator<<( std::ostream & o, Server const& i )
{
	o	<< "\n\t** Server **" << std::endl
		<<  "worker_connection: " << i.getWorkCont() << std::endl
		<<  "client_max_body_size: " << i.getClientMaxBody() << std::endl
		<< "host: " << i.getHost() << std::endl
		<< "port: " << i.getPort() << std::endl
		<< "root: " << i.getRoot() << std::endl;
	o	<< "indexes: \n\t";
	std::vector<std::string>::const_iterator it_s;
	for (it_s = i.getIndexes().begin(); it_s != i.getIndexes().end(); ++it_s)
	{
		o << *it_s << " ";
	}
	o	<< "\nserver names: \n\t";
	for (it_s = i.getServerNames().begin(); it_s != i.getServerNames().end(); ++it_s)
	{
		o << *it_s << " ";
	}
	o	<< "\nerror pages: \n";
	std::map<int, std::string>::const_iterator it;
	for (it = i.getErrorPages().begin(); it != i.getErrorPages().end(); ++it)
	{
		std::cout  << "\t" << utils::itos(it->first) << ": " << it->second << std::endl;
	}
	o << std::endl;

	std::vector<Location> locations = i.getLocations();
	o << "Locations:  " << locations.size() << std::endl;
	for (size_t i = 0; i < locations.size(); i++)
	{
		o << locations[i] << std::endl;
	}

	return o;
}

/*
** --------------------------------- METHODS ----------------------------------
*/

void		Server::defaultServer( void )
{
	setRoot("var/www/html");
	setIndex("index.42webserver.html");
}

/** Function to handle requered method and prepare a responce for further sending */

void	Server::initResponse(Response& response, const Request& request)
{
	response.protocol = request.protocol;
	response.location_found = false;
	response.method = request.method;
	response.reqURI = canonicalizePath(request.reqURI);
	response.server = "42_webserv";
	response.content_lenght = 0;
	response.status_code = 0;
	response.host = request.host;
}

void	Server::handleStaticRequest(const Request& request, Response& response, const Location& loc)
{
	// std::cout << "request.method " << request.method << std::endl;
	if (std::strcmp(utils::str_tolower(request.method).c_str(), "get") == 0 || \
		std::strcmp(utils::str_tolower(request.method).c_str(), "head") == 0)
	{
		handleGET(response, loc);
	}
	else if (std::strcmp(utils::str_tolower(request.method).c_str(), "post") == 0)
	{
		handlePOST(request, response, loc);
	}
	else if (std::strcmp(utils::str_tolower(request.method).c_str(), "delete") == 0)
	{
		handleDELETE(response, loc);
	}
	else
	{
		/* 501 Not Implemented */
		response.error_path = getCustomErrorPage(NOT_IMPLEMENTED, loc);
		setErrorResponse(response, NOT_IMPLEMENTED);
	}
}

void		Server::handleGET(Response& response, const Location& loc)
{
	/** 404 Not Found */
	if (!response.location_found)
	{
		response.error_path = getCustomErrorPage(NOT_FOUND, loc);
		setErrorResponse(response, NOT_FOUND);
	}
	/** 405 Method Not Allowed */
	else if (!utils::is_str_in_vector(response.method, loc.getAllowedMethods()))
	{
		handleMethodNotAllowed(response, loc);
	}
	else
	{
		handleRequestedURI(response, loc);
	}
}

void		Server::handleDELETE(Response& response, const Location& loc)
{
	if (!loc.getRedirect().empty())
	{
		handleAndSetRedirectResponse(response, loc);
		return;
	}
	response.path = determineFilePath(response.reqURI, &loc);

	/** 404 Not Found */
	if (!response.location_found)
	{
		response.error_path = getCustomErrorPage(NOT_FOUND, loc);
		setErrorResponse(response, NOT_FOUND);
	}
	/** 405 Method Not Allowed */
	else if (!utils::is_str_in_vector(response.method, loc.getAllowedMethods()))
	{
		handleMethodNotAllowed(response, loc);
	}
	/** 403 Forbidden */
	else if (!utils::has_write_permission(response.path))
	{
		response.error_path = getCustomErrorPage(FORBIDDEN, loc);
		setErrorResponse(response, FORBIDDEN);
	}
	else
	{
		if (utils::is_regular_file(response.path))
		{
			response.status_code = remove_file(response.path);
			response.error_path = getCustomErrorPage(response.status_code, loc);
			setDeleteResponse(response, response.status_code);
		}
		else if (utils::is_directory(response.path))
		{
			response.status_code = handleDeleteDirectoryResponse(response);
			response.error_path = getCustomErrorPage(response.status_code, loc);
			setDeleteResponse(response, response.status_code);
		}
		else
		{
			response.error_path = getCustomErrorPage(NOT_FOUND, loc);
			setErrorResponse(response, NOT_FOUND);
		}
	}
}

void		Server::handlePOST(const Request& request, Response& response, const Location& loc)
{
	response.uploadDir = searchingUploadDir(response.reqURI, &loc);
	response.path = determineFilePath(response.reqURI, &loc);
	/** 404 Not Found */
	if (!response.location_found)
	{
		response.error_path = getCustomErrorPage(NOT_FOUND, loc);
		setErrorResponse(response, NOT_FOUND);
	}
	/** 405 Method Not Allowed */
	else if (!utils::is_str_in_vector(response.method, loc.getAllowedMethods()))
	{
		handleMethodNotAllowed(response, loc);
	}
	else
	{
		// std::cout << "response1 " << response << std::endl;
		if (utils::substr_after_rdel(response.path, ".").empty())
		{
			response.error_path = getCustomErrorPage(FORBIDDEN, loc);
			setErrorResponse(response, FORBIDDEN);
			return;
		}
		response.uploadFile = utils::substr_after_rdel(response.path, "/");
		std::cout << "response.uploadFile1 " << response.uploadFile << std::endl;
		std::ofstream file((response.uploadDir + response.uploadFile).c_str()); // "/" ??
		if (!file.is_open())
		{
			std::cerr	<< "Failed to open file: " << response.path
						<< " (" << strerror(errno) << ")" << std::endl;

			if (errno == EACCES || errno == EROFS || errno == ENOENT)
			{
				response.error_path = getCustomErrorPage(FORBIDDEN, loc);
				setErrorResponse(response, FORBIDDEN);
			}
			else
			{
				response.error_path = getCustomErrorPage(INTERNAL_SERVER_ERROR, loc);
				setErrorResponse(response, INTERNAL_SERVER_ERROR);
			}

			return;
		}

		file << request.reqBody;
		if (file.fail())
		{
			std::cerr << "Error writing to file: " << response.uploadDir + response.uploadFile << std::endl;
			response.error_path = getCustomErrorPage(INTERNAL_SERVER_ERROR, loc);
			setErrorResponse(response, INTERNAL_SERVER_ERROR);
			file.close();
			return;
		}
		file.close();
		setPostResponse(response, CREATED);
	}

}

void Server::handleMethodNotAllowed(Response& response, const Location& location)
{
	response.allow = utils::vector_tostr(location.getAllowedMethods());
	response.error_path = getCustomErrorPage(METHOD_NOT_ALLOWED, location);
	setErrorResponse(response, METHOD_NOT_ALLOWED);
}

void		Server::handleRequestedURI(Response& response, const Location& loc)
{
	if (!loc.getRedirect().empty())
	{
		handleAndSetRedirectResponse(response, loc);
		return;
	}
	response.path = determineFilePath(response.reqURI, &loc);
	// std::cout << "determineFilePath " << response.path << std::endl;
	if (utils::is_regular_file(response.path))
	{
		setGetResponse(response, OK);
	}
	else
	{
		handleGetDirectoryResponse(response, loc);
	}
}

void	Server::setCGIResponse(Response& response, size_t status_code)
{
	(void)response;
	(void)status_code;
}


/** NOTES: to rm
 * - parse cgi request
 * - path also a cgi path to the executable file without query
 * -
 */

int		Server::handleCGI(Response& response, Location& loc)
{
	response.uploadDir = utils::substr_before_rdel(response.path, "/");
	response.uploadDir = Server::searchingUploadDir(response.reqURI, &loc);
	if (utils::is_regular_file(response.path))
	{
		if (!response.location_found || !utils::is_matching_ext(response.path, loc.getCGIExtension()))
		{
			return 1;
		}
		response.uploadFile = utils::substr_after_rdel(response.path, "/");
		std::cout << "response.uploadFile1 " << response.uploadFile << std::endl;
		// setCGIResponse(response, OK);
		setGetResponse(response, OK);
		return 0;
	}
	else if (!utils::is_directory(response.path))
	{
		if (response.location_found && appendIndexFile(response.path, loc))
		{
			// response.uploadDir = substr_before_rdel(response.path, "/");
			response.uploadFile = utils::substr_after_rdel(response.path, "/");
			if (utils::is_regular_file(response.path))
			{
				if (!utils::is_matching_ext(response.path, loc.getCGIExtension()))
				{
					return 1;
				}
				response.uploadFile = utils::substr_after_rdel(response.path, "/");
				// setCGIResponse(response, OK);
				setGetResponse(response, OK);
				return 0;
			}
			else if (!utils::is_directory(response.path))
			{
				return 3;
			}
			else
			{
				return 5;
			}
		}
		else
		{
			return 2;
		}
	}
	return 4;
}

/** Based on a requested path searching for the location */
bool		Server::findRequestedLocation(const std::string& path, Location& loc)
{
	bool location_found;

	location_found = searchingExtensionMatchLocation(path, loc);
	if (!location_found)
	{
		location_found = searchingPrefixMatchLocation(path, loc);
	}

	// std::cout << "location:" << loc;

	return location_found;
}

void	Server::setErrorResponse(Response& response, size_t status_code)
{
	response.status_code = status_code;
	response.reason_phrase = utils::get_reason_phrase(status_code);
	if (!response.error_path.empty())
		response.content = utils::load_file_content(response.error_path);
	else
		response.content = utils::generate_html_error_page(status_code);
	response.content_lenght = response.content.length();
	response.content_type = utils::get_MIME_type(response.content);
}

void	Server::setGetResponse(Response& response, size_t status_code)
{
	response.content = utils::load_file_content(response.path);
	response.content_lenght = response.content.length();
	// std::cout << "response.content_lenght " << response.content_lenght << std::endl;
	if (response.content_lenght <= 0)
	{
		status_code = NO_CONTENT;
	}
	response.content_type = utils::get_MIME_type(response.path);
	response.status_code = status_code;
	response.reason_phrase = utils::get_reason_phrase(status_code);
}

void	Server::setPostResponse(Response& response, size_t status_code)
{
	response.status_code = status_code;
	response.reason_phrase = utils::get_reason_phrase(status_code);
}

void	Server::setDeleteResponse(Response& response, size_t status_code)
{
	if (utils::is_client_error(status_code) || utils::is_server_error(status_code))
	{
		setErrorResponse(response, status_code);
		return;
	}
	response.status_code = status_code;
	response.reason_phrase = utils::get_reason_phrase(status_code);
}

/** formats path if requied, and use a path as a file, otherwise to generate default */
void	Server::handleAndSetRedirectResponse(Response& response, const Location& loc)
{
	if (!loc.getRedirect().empty())
	{
		std::map<int, std::string>::const_iterator it = loc.getRedirect().begin();
		response.status_code = it->first;
		response.location = it->second; // "http://localhost:8080" +
		// response.path = it->second; // what with root???
		// searchingPrefixMatchLocation(it->second, path, location, location_found);
	}
	// searchingPrefixMatchLocation(response.path, response.path, loc, response.location_found);
	if (utils::is_regular_file(response.path))
	{
		setGetResponse(response, response.status_code);
	}
	else
	{
		response.error_path = getCustomErrorPage(response.status_code, loc);
		setErrorResponse(response, response.status_code);
	}
}

void	Server::handleGetDirectoryResponse(Response& response, const Location& loc)
{
	// std::cout << "response.path " << response.path << std::endl;
	if (!utils::is_directory(response.path))
	{
		response.error_path = getCustomErrorPage(NOT_FOUND, loc);
		setErrorResponse(response, NOT_FOUND);
	}
	else if (!utils::ends_with(response.path, "/"))
	{
		// path += "/"; // Redirect by appending '/' /** ?? */
		response.location = response.reqURI + "/"; /** ideally change to scheme "http://localhost:8080" */ // "http://localhost:8080" +
		response.error_path = getCustomErrorPage(MOVED_PERMANENTLY, loc);
		setErrorResponse(response, MOVED_PERMANENTLY);
	}
	else if (appendIndexFile(response.path, loc))
	{
		setGetResponse(response, OK);
	}
	else if (!loc.getAutoindex())
	{
		response.error_path = getCustomErrorPage(FORBIDDEN, loc);
		setErrorResponse(response, FORBIDDEN);
	}
	else
	{
		response.path = utils::generate_html_directory_listing(response.path);
		setGetResponse(response, OK);
	}
}

/** @return status code */
size_t	Server::handleDeleteDirectoryResponse(Response& response)
{
	if (!utils::ends_with(response.path, "/"))
	{
		return CONFLICT;
	}
	else if (!utils::has_write_permission(response.path))
	{
		return FORBIDDEN;
	}
	else
	{
		return remove_directory(response.path);
	}
}

size_t	Server::remove_file(const std::string& path)
{
	if (std::remove(path.c_str())  == 0)
	{
		return NO_CONTENT;
	}
	return INTERNAL_SERVER_ERROR;
}

size_t	Server::remove_directory(const std::string& path)
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

void	Server::createResponse(const Response& response, std::string& result)
{
	result = "";
	std::string SP = " ";
	std::string CRLF = "\r\n";

	result.append(response.protocol);
	result.append(SP);
	result.append(utils::itos(response.status_code));
	result.append(SP);
	result.append(response.reason_phrase);
	result.append(CRLF);
	result.append("Date:");
	result.append(SP);
	result.append(utils::formatDate(utils::get_timestamp("")));
	result.append(CRLF);
	result.append("Server:");
	result.append(SP);
	result.append(response.server);
	result.append(CRLF);
	result.append("Last-Modified:");
	result.append(SP);
	result.append(utils::formatDate(utils::get_timestamp(response.path)));
	result.append(CRLF);
	if (response.status_code == METHOD_NOT_ALLOWED)
	{
		result.append("Allow:");
		result.append(SP);
		result.append(response.allow);
		result.append(CRLF);
	}
	if (response.status_code != NO_CONTENT && !utils::is_informational(response.status_code))
	{
		result.append("Content-Type:");
		result.append(SP);
		result.append(response.content_type);
		result.append(CRLF);
		result.append("Content-Length:");
		result.append(SP);
		result.append(utils::itos(response.content_lenght));
		result.append(CRLF);
	}
	if (!response.location.empty())
	{
		result.append("Location:");
		result.append(SP);
		result.append(response.location);
		result.append(CRLF);
	}
	if (std::strcmp(utils::str_tolower(response.method).c_str(), "post") != 0)
	{
		result.append(CRLF);
		result.append(response.content);
	}
}

/** Check for custom error pages if any, othewise generate default html error page */
std::string		Server::getCustomErrorPage(int status_code, const Location& src)
{
	std::string					path = std::string();
	Location					loc;
	bool						location_found = false;
	std::map<int, std::string>	er_pages;

	er_pages = src.getErrorPages();
	if (er_pages.size() == 0 && !this->getErrorPages().empty())
	{
		er_pages = this->getErrorPages();
	}

	if (er_pages.size() > 0 && er_pages.find(status_code) != er_pages.end())
	{
		if ((location_found = findRequestedLocation(er_pages[status_code], loc)) == true)
		{
			path = determineFilePath(er_pages[status_code], &loc);
		}
		// std::cout << "std::string	Server::getCustomErrorPage(int status_code, const Location& src)" << std::endl;
	}
	return path;
}

bool		Server::appendIndexFile(std::string& path, const Location& loc)
{
	std::vector<std::string> idxs = loc.getIndexes();
	if (idxs.size() == 0)
	{
		idxs = this->getIndexes();
	}
	std::vector<std::string> dir_content;

	utils::get_dir_entries(path, dir_content);

	for (size_t i = 0; i < idxs.size(); i++)
	{
		if (utils::is_str_in_vector(idxs[i], dir_content))
		{
			path += idxs[i];
			return true;
		}
	}

	return false;
}

// uri: /dir/*.bla */
bool		Server::searchingExtensionMatchLocation(std::string requested_path, Location& location)
{
	size_t pos = 0;

	for (int i = 0; i < this->_location_nbr; i++)
	{
		std::string loc_path = this->_locations[i].getPath();
		if ((pos = loc_path.find('*')) == std::string::npos)
			continue;
		std::string ext = loc_path.substr(pos + 1);
		loc_path = loc_path.substr(0, pos);
		if (!utils::ends_with(requested_path, ext))
			continue;
		if (!loc_path.empty() && std::strncmp(loc_path.c_str(), requested_path.c_str(), loc_path.length()) != 0)
			continue;
		location = this->_locations[i];
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
bool		Server::searchingPrefixMatchLocation(std::string requested_path, Location& location)
{
	// if (requested_path.empty())
	// {
	// 	requested_path = "/";
	// }

	std::string	searched_path = requested_path;
	std::string	rest = std::string();
	bool	location_found = false;
	size_t	pos = 0;

	while (!searched_path.empty())
	{
		for (int i = 0; i < this->_location_nbr; i++)
		{
			std::string loc_path = this->_locations[i].getPath();
			if (std::strcmp(searched_path.c_str(), loc_path.c_str()) == 0)
			{
				location = this->_locations[i];
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

std::string			Server::decodeURI(const std::string& path)
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


std::string			Server::canonicalizePath(const std::string& path)
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

/** Formates the relative requested URI based on root and alias directive from server and location block */
std::string			Server::determineFilePath(std::string requested_path, const Location* loc)
{
	std::string path = requested_path;
	std::string	root = std::string();
	// std::cout << "location: " << *loc << std::endl;
	if (loc && !loc->getRoot().empty())
	{
		root = loc->getRoot();
	}
	else if (!this->getRoot().empty())
	{
		root = this->getRoot();
	}

	if (loc && !loc->getAlias().empty())
	{
		std::string	loc_path = loc->getPath();
		if (path.find(loc_path) == 0)
		{
			root = loc->getAlias();
			path.erase(0, loc_path.length());
		}
	}
	std::string full_path = root + path;
	return full_path;
}

/** if specified uplaod_dir directive will appendet to root, otherwise */
std::string		Server::searchingUploadDir(std::string requested_path, const Location* loc)
{
	std::string p = std::string();
	std::string root = std::string();
	std::string full_path = std::string();
	Location	tmp_loc;

	if (loc && !loc->getUploadDir().empty())
	{
		p =  loc->getUploadDir();
		bool location_found = findRequestedLocation(p, tmp_loc);
		if (location_found)
		{
			full_path = determineFilePath(p, &tmp_loc);
		}
		full_path = determineFilePath(p, loc);
	}
	else
	{
		p = utils::substr_before_rdel(requested_path, "/");
		full_path = determineFilePath(p, loc);
	}
	return full_path;
}



/*
** --------------------------------- ACCESSOR ---------------------------------
*/

void Server::setWorkCont(int connections)
{
	_worker_connections = connections;
}

void Server::addLocationNbr(int value)
{
	_location_nbr += value;
}

void	Server::setClientMaxBody(int value)
{
	_client_max_body_size = value;
}

void	Server::setHost(const std::string &arg)
{
	_host = arg;
}

void	Server::setPort( int val )
{
	_port = val;
}

void	Server::setRoot(const std::string &arg)
{
	_root = arg;
}

void	Server::setErrorLog(const std::string &arg)
{
	_error_log = arg;
}

void	Server::addErrorPage(int key, const std::string &value)
{
	_error_pages[key] = value;
}

void	Server::setIndex(const std::string &arg)
{
	_indexes.push_back(arg);
}

void	Server::setServerName(const std::string &arg)
{
	_server_names.push_back(arg);
}

void	Server::setLocation(const Location &src)
{
	_locations.push_back(src);
	_location_nbr += 1;
}

int Server::getWorkCont( void ) const
{
	return (_worker_connections);
}

int Server::getLocationNbr( void ) const
{
	return (_location_nbr);
}

int	Server::getClientMaxBody( void ) const
{
	return (_client_max_body_size);
}

const std::string&	Server::getHost( void ) const
{
	return (_host);
}

int		Server::getPort( void ) const
{
	return (_port);
}

const std::string&	Server::getRoot( void ) const
{
	return (_root);
}

const std::string&	Server::getErrorLog( void ) const
{
	return (_error_log);
}

const std::map<int, std::string>&	Server::getErrorPages( void ) const
{
	return (_error_pages);
}

const std::vector<std::string>&	Server::getIndexes( void ) const
{
	return (_indexes);
}

const std::vector<std::string>&	Server::getServerNames( void ) const
{
	return (_server_names);
}

const std::vector<Location>&	Server::getLocations( void ) const
{
	return (_locations);
}

/* ************************************************************************** */
