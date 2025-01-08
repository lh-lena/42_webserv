#include "../includes/Server.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Server::Server()
	:	
		_location_nbr(0),
		_client_max_body_size(1 * 1024 * 1024), //1M
		_worker_connections(1024),
		_port(80),
		_host("localhost"),
		_root("/var/www/html"),
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
		std::cout  << "\t" << intToStr(it->first) << ": " << it->second << std::endl;
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

/** Append an HTTP rel-path to a server root path.
 *  The created path is saved in a path parameter is normalized for the platform.
 * path argument will be filled with a data or error page for a new responce
 * return status code
*/

int		Server::handleRequestedURI(std::string requested_path, std::string& path)
{
	Location loc;
	normalizeURI(requested_path, path, loc);
	

	return 0;
}

/**
 * Normalize a URI by resolving its root and determining the relevant location.
 * 
 * @param requested_path - The requested URI path to handle.
 * @param path - The normalized path will be stored here.
 * @param location - The relevant Location object will be stored here.
 * @return 0 on success, or an error code if needed.
 */

int		Server::normalizeURI(std::string requested_path, std::string& path, Location& location)
{
	if (requested_path.empty())
	{
		requested_path = "/";
	}

	std::string root = this->_root;
	path = "";
	size_t pos = 0;
	std::string searched_path = requested_path;
	std::string rest = "";
	bool location_found = false;

	while (!searched_path.empty())
	{
		std::cout << "searched_path  : " << searched_path << "\n";
		for (int i = 0; i < this->_location_nbr; i++)
		{
			std::string loc_path = this->_locations[i].getPath();
			if (searched_path == loc_path)
			{
				if (!this->_locations[i].getRoot().empty())
				{
					root = _locations[i].getRoot();
					location = _locations[i];
				}
				location_found = true;
				break;
			}
		}
		std::cout << "root  : " << root << "\n";
		if (location_found)
			break;
		pos = searched_path.rfind("/");
		if (searched_path != "/")
		{
			rest = requested_path.substr(pos);
			searched_path = requested_path.erase(pos);
		}
	}
	path = root + rest;
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