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
		_host("localhost"),
		_root("var/www/html"), //?
		_error_log("error.log")
{
	_indexes.push_back("index.html");
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
		std::cout  << "\t" << itos(it->first) << ": " << it->second << std::endl;
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

/** Function to handle requered method and prepare a responce for further sending */

void	Server::handleRequestMethod(const Request& request, Response& response)
{
	std::cout << request.method_r << std::endl;
	if (std::strcmp(str_tolower(request.method_r).c_str(), "get") == 0 || \
		std::strcmp(str_tolower(request.method_r).c_str(), "head") == 0)
	{
		handleGET(request, response);
	}
	else if (std::strcmp(str_tolower(request.method_r).c_str(), "post") == 0)
	{
		handlePOST(request, response);
	}
	else if (std::strcmp(str_tolower(request.method_r).c_str(), "delete") == 0)
	{
		handleDELETE(request, response);
	}
	else
	{
		/* 501 Not Implemented */
		setErrorResponse(response, NOT_IMPLEMENTED);
	}
}

void	Server::initResponse(Response& response, const Request& request)
{
	response.protocol = "HTTP/1.1";
	response.location_found = false;
	response.method = request.method_r;
	response.reqURI = request.reqURI;
	response.server = "42_webserv";
	response.content_lenght = 0;
	response.status_code = 0;
	response.location = "";
}

void		Server::handleGET(const Request& request, Response& response)
{
	Location		location;

	initResponse(response, request);
	response.status_code = searchingExtensionMatchURI(request.reqURI, response.path, location, response.location_found); //added
	if (response.status_code == -1)
	{
		response.status_code = searchingPrefixMatchURI(request.reqURI, response.path, location, response.location_found);
	}
	std::cout << "response.status_code " << response.status_code << std::endl;
	/** 404 Not Found */
	if (!response.location_found)
	{
		setErrorResponse(response, NOT_FOUND);
	}
	/** 405 Method Not Allowed */
	else if (!is_str_in_vector(response.method, location.getAllowedMethods()))
	{
		handleMethodNotAllowed(response, location);
	}
	else
	{
		handleRequestedURI(response, location);
	}
/* 	else if (starts_with(response.path, "/cgi-bin/"))
	{
		handleCGI(response, location);
	} */
}

void		Server::handleDELETE(const Request& request, Response& response)
{
	Location		location;

	initResponse(response, request);
	response.status_code = searchingExtensionMatchURI(request.reqURI, response.path, location, response.location_found); //added
	if (response.status_code == -1)
	{
		response.status_code = searchingPrefixMatchURI(request.reqURI, response.path, location, response.location_found);
	}
	std::string path = response.path;

	/** 404 Not Found */
	if (!response.location_found)
	{
		setErrorResponse(response, NOT_FOUND);
	}
	/** 405 Method Not Allowed */
	else if (!is_str_in_vector(response.method, location.getAllowedMethods()))
	{
		handleMethodNotAllowed(response, location);
	}
	/** 403 Forbidden */
	else if (!has_write_permission(path))
	{
		setErrorResponse(response, FORBIDDEN);
	}
	else
	{
		if (is_regular_file(path))
		{
			response.status_code = remove_file(path);
			setDeleteResponse(response, response.status_code);
		}
		else if (is_directory(path))
		{
			response.status_code = handleDeleteDirectoryResponse(response);
			setDeleteResponse(response, response.status_code);
		}
		else
		{
			setErrorResponse(response, NOT_FOUND);
		}
	}
}

void		Server::handlePOST(const Request& request, Response& response)
{
	Location		location;

	initResponse(response, request);
	response.status_code = searchingExtensionMatchURI(request.reqURI, response.path, location, response.location_found); //added
	if (response.status_code == -1)
	{
		response.status_code = searchingPrefixMatchURI(request.reqURI, response.path, location, response.location_found);
	}

	searchingUploadDir(response.reqURI, response.uploadDir, location, response.location_found);
	std::cout << "response.uploadDir " << response.uploadDir << std::endl;
	std::cout << "response.uploadDir " << response.uploadFile << std::endl;
	/** 404 Not Found */
	if (!response.location_found)
	{
		setErrorResponse(response, NOT_FOUND);
	}
	/** 405 Method Not Allowed */
	else if (!is_str_in_vector(response.method, location.getAllowedMethods()))
	{
		handleMethodNotAllowed(response, location);
	}
	else
	{
		/** if the path not a dir ??*/
		if (substr_after_rdel(response.path, ".").empty())
		{
			setErrorResponse(response, FORBIDDEN);
			return;
		}
		response.uploadFile = substr_after_rdel(response.path, "/");
		std::ofstream file((response.uploadDir + response.uploadFile).c_str()); // "/" ??
		if (!file.is_open())
		{
			std::cerr	<< "Failed to open file: " << response.path 
						<< " (" << strerror(errno) << ")" << std::endl;

			if (errno == EACCES || errno == EROFS || errno == ENOENT)
			{
				setErrorResponse(response, FORBIDDEN);
			}
			else
			{
				setErrorResponse(response, INTERNAL_SERVER_ERROR);
			}

			return;
		}

		file << request.reqBody;
		if (file.fail())
		{
			std::cerr << "Error writing to file: " << response.uploadDir + response.uploadFile << std::endl;
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
	response.allow = vector_tostr(location.getAllowedMethods());
	setErrorResponse(response, METHOD_NOT_ALLOWED);
}

void		Server::handleRequestedURI(Response& response, Location& loc)
{
	if (response.status_code != 0)
	{
		handleAndSetRedirectResponse(response, loc);
	}
	else if (is_regular_file(response.path))
	{
		setGetResponse(response, OK);
	}
	else
	{
		handleGetDirectoryResponse(response, loc);
	}
}

/** TODO:
 * - parse cgi request
 */

int		Server::handleCGI(Response& response, Location& loc)
{
	std::cout << "response.status_code " << response.status_code << std::endl;

	// if (response.status_code != 0)
	// {
	// 	handleAndSetRedirectResponse(response, loc);
	// }
	searchingUploadDir(response.reqURI, response.uploadDir, loc, response.location_found);
	
	if (response.path.find("?")) /* check queries */
	{
		response.query = substr_after_rdel(response.path, "?");
		response.path = substr_befor_rdel(response.path, "?");
	}
	if (is_regular_file(response.path))
	{
		if (!is_matching_ext(response.path, loc.getCgiExtension()))
		{
			// setCGIResponse(response, NOT_FOUND);
			return 1;
		}
		response.uploadFile = substr_after_rdel(response.path, "/");
		setCGIResponse(response, OK);
		return 0;
	}
	else
	{
		if (appendIndexFile(response.path, loc))
		{
			response.uploadDir = substr_befor_rdel(response.path, "/");
			response.uploadFile = substr_after_rdel(response.path, "/");
		}
	}
	return 2;
}

void	Server::setCGIResponse(Response& response, size_t status_code)
{
	(void)response;
	(void)status_code;
}

void	Server::setErrorResponse(Response& response, size_t status_code)
{
	response.status_code = status_code;
	response.reason_phrase = get_reason_phrase(status_code);
	response.content = generate_html_error_page(status_code);
	response.content_lenght = response.content.length();
	response.content_type = get_MIME_type(response.content);
}

void	Server::setGetResponse(Response& response, size_t status_code)
{
	response.content = get_file_content(response.path);
	response.content_lenght = response.content.length();
	std::cout << "response.content_lenght " << response.content_lenght << std::endl;
	if (response.content_lenght <= 0)
	{
		status_code = NO_CONTENT;
	}
	response.content_type = get_MIME_type(response.path);
	response.status_code = status_code;
	response.reason_phrase = get_reason_phrase(status_code);
}

void	Server::setPostResponse(Response& response, size_t status_code)
{
	response.status_code = status_code;
	response.reason_phrase = get_reason_phrase(status_code);
}

void	Server::setDeleteResponse(Response& response, size_t status_code)
{
	if (is_client_error(status_code) || is_server_error(status_code))
	{
		setErrorResponse(response, status_code);
		return;
	}
	response.status_code = status_code;
	response.reason_phrase = get_reason_phrase(status_code);
}

/** formats path if requied, and use a path as a file, otherwise to generate default */
void	Server::handleAndSetRedirectResponse(Response& response, Location& loc)
{
	(void)loc;
	searchingPrefixMatchURI(response.path, response.path, loc, response.location_found);
	if (is_regular_file(response.path))
	{
		setGetResponse(response, response.status_code);
	}
	else
	{
		setErrorResponse(response, response.status_code);
	}
}

void	Server::handleGetDirectoryResponse(Response& response, Location& loc)
{
	std::cout << "response.path " << response.path << std::endl;
	if (!is_directory(response.path))
	{
		setErrorResponse(response, NOT_FOUND);
	}
	else if (!ends_with(response.path, "/"))
	{
		// path += "/"; // Redirect by appending '/' /** ?? */
		response.location = "http://localhost:8080" + response.reqURI + "/"; /** ideally change to scheme "http://localhost:8080" */
		setErrorResponse(response, MOVED_PERMANENTLY);
	}
	else if (appendIndexFile(response.path, loc))
	{
		setGetResponse(response, OK);
	}
	else if (!loc.getAutoindex())
	{
		setErrorResponse(response, FORBIDDEN);
	}
	else
	{
		response.path = generate_html_directory_listing(response.path);
		setGetResponse(response, OK);
	}
}

size_t	Server::handleDeleteDirectoryResponse(Response& response)
{
	if (!ends_with(response.path, "/"))
	{
		return CONFLICT;
	}
	else if (!has_write_permission(response.path))
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
	if (get_dir_entries(path, cont) == 0)
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
	result.append(itos(response.status_code));
	result.append(SP);
	result.append(response.reason_phrase);
	result.append(CRLF);
	result.append("Date:");
	result.append(SP);
	result.append(formatDate(get_timestamp("")));
	result.append(CRLF);
	result.append("Server:");
	result.append(SP);
	result.append(response.server);
	result.append(CRLF);
	result.append("Last-Modified:");
	result.append(SP);
	result.append(formatDate(get_timestamp(response.path)));
	result.append(CRLF);
	if (response.status_code == METHOD_NOT_ALLOWED)
	{
		result.append("Allow:");
		result.append(SP);
		result.append(response.allow);
		result.append(CRLF);
	}
	if (response.status_code != NO_CONTENT && !is_informational(response.status_code))
	{
		result.append("Content-Type:");
		result.append(SP);
		result.append(response.content_type);
		result.append(CRLF);
		result.append("Content-Length:");
		result.append(SP);
		result.append(itos(response.content_lenght));
		result.append(CRLF);
	}
	if (!response.location.empty())
	{
		result.append("Location:");
		result.append(SP);
		result.append(response.location);
		result.append(CRLF);
	}
	if (std::strcmp(str_tolower(response.method).c_str(), "post") != 0)
	{
		result.append(CRLF);
		result.append(response.content);
	}
}


std::string	Server::formatDate(time_t timestamp)
{
	/** RFC7231: IMF-fixdate  = day-name "," SP date1 SP time-of-day SP GMT */

	struct tm datetime;
	char buf[100];

	gmtime_r(&timestamp, &datetime);
	std::strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", &datetime);

	return std::string(buf);
}

/** Check for custom error pages if any, othewise generate default html error page */
std::string		Server::handleErrorPageResponse(int status_code, const Location& src)
{
	std::string					path;
	Location					loc;
	bool						found_location;
	std::map<int, std::string>	er_pages;

	er_pages = src.getErrorPages();
	if (er_pages.size() == 0)
	{
		er_pages = this->getErrorPages();
	}

	if (er_pages.size() > 0 && er_pages.find(status_code) != er_pages.end())
	{
		searchingPrefixMatchURI(er_pages[status_code], path, loc, found_location);
		return path;
	}
	path = generate_html_error_page(status_code);
	return path;
}

bool		Server::appendIndexFile(std::string& path, const Location& loc)
{
	std::vector<std::string> idxs = loc.getIndexes();
	std::vector<std::string> dir_content;

	get_dir_entries(path, dir_content);

	for (size_t i = 0; i < idxs.size(); i++)
	{
		if (is_str_in_vector(idxs[i], dir_content))
		{
			path += idxs[i];
			return true;
		}
	}

	return false;
}

// uri: /dir/*.bla */
int		Server::searchingExtensionMatchURI(std::string requested_path, std::string& path, Location& location, bool& location_found)
{
	location_found = false;
	size_t pos = 0;
	std::string root = this->_root;
	path = "";

	for (int i = 0; i < this->_location_nbr; i++)
	{
		std::string loc_path = this->_locations[i].getPath();
		if ((pos = loc_path.find('*')) == std::string::npos)
			continue;
		std::string ext = loc_path.substr(pos + 1);
		loc_path = loc_path.substr(0, pos);
		if (!ends_with(requested_path, ext))
			continue;
		if (!loc_path.empty() && std::strncmp(loc_path.c_str(), requested_path.c_str(), loc_path.length()) != 0)
			continue;
		location = this->_locations[i];
		location_found = true;
		if (!location.getReturn().empty())
		{
			std::map<int, std::string>::const_iterator it = location.getReturn().begin();
			path = it->second;
			std::cout << path << std::endl;
			return it->first;
		}
		if (!location.getRoot().empty())
		{
			root = location.getRoot();
		}
		if (!location.getUploadDir().empty())
		{
			root = location.getUploadDir();
		}
		if (!root.empty() && std::strncmp(loc_path.c_str(), requested_path.c_str(), loc_path.length()) == 0)
		{
			path = root + requested_path;
			return 0;
		}
	}
	return -1;
}

/**
 * Normalize a URI by resolving its root and determining the relevant prefix-based location blocks.
 * 
 * @param requested_path - The requested URI path to handle.
 * @param path - The normalized path will be stored here.
 * @param location - The relevant Location object will be stored here.
 * @return 0 on success, or a redirect code if "return" detected
 */

int		Server::searchingPrefixMatchURI(std::string requested_path, std::string& path, Location& location, bool& location_found)
{
	if (requested_path.empty())
	{
		requested_path = "/";
	}

	size_t				pos = 0;
	std::string 		rest = "";
	std::string 		root = this->_root;
	std::string 		searched_path = requested_path;

	path = "";
	location_found = false;

	while (!searched_path.empty())
	{
		for (int i = 0; i < this->_location_nbr; i++)
		{
			std::string loc_path = this->_locations[i].getPath();
			if (std::strcmp(searched_path.c_str(), loc_path.c_str()) == 0)
			{
				location = this->_locations[i];
				location_found = true;
				if (!location.getReturn().empty())
				{
					std::map<int, std::string>::const_iterator it = location.getReturn().begin();
					path = it->second;
					// searchingPrefixMatchURI(it->second, path, location, location_found);
					return it->first;
				}
				if (!location.getRoot().empty())
				{
					root = location.getRoot();
				}
				break;
			}
		}
		pos = searched_path.rfind("/");
		if (pos != std::string::npos && ends_with(searched_path, "/"))
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
	path = root + searched_path + rest;
	std::cout << "\t Path2 " << path << "\n";
	std::cout << location << "\n";
	return 0;
}

int		Server::searchingUploadDir(std::string requested_path, std::string& path, Location& location, bool& location_found)
{
	if (!location_found)
	{
		return -1;
	}

	std::string p = "";
	std::string root = this->getRoot();
	if (!location.getUploadDir().empty())
	{
		p = location.getUploadDir(); /** what if there another root as well? */
		std::cout <<  "upload dir-> " << p << std::endl;
	}
	else
	{
		p = substr_befor_rdel(requested_path, "/");
	}
	if (!location.getRoot().empty())
	{
		root = location.getRoot();
		std::cout <<  " root upload dir-> " << root << std::endl;
	}
	path = root + p;
	std::cout <<  " path upload dir-> " << path << std::endl;
	return 0;
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