#include "../includes/Server.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Server::Server()
	:	
		_location_nbr(0),
		_client_max_body_size(1 * 1024 * 1024), //1M
		_worker_connections(1024),
		_host("localhost"),
		_port("8080"),
		_root("./var/www/html"),
		_error_log("error.log")
{
	// _indexes.push_back("index.html");
	// _indexes.push_back("index.htm");
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
		<<  "worker_connection " << i.getWorkCont() << std::endl
		<<  "client_max_body_size " << i.getClientMaxBody() << std::endl
		<< "host " << i.getHost() << std::endl
		<< "port " << i.getPort() << std::endl
		<< "root " << i.getRoot() << std::endl
		<< "indexes <vector>: " << std::endl; 
	for (std::string s : i.getIndexes())
		o << s << " ";
	o	<< "\nserver names <vector>: " << std::endl;
	for (std::string s : i.getServerNames())
		o << s << " ";

	o << std::endl;

	std::vector<Location> locations = i.getLocations();
	o << "Locations:  " << locations.size() << std::endl;
	for (Location loc : locations)
	{
		o << loc << std::endl;
	}

	return o;
}

/*
** --------------------------------- METHODS ----------------------------------
*/


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

void	Server::setPort(const std::string &arg)
{
	_port = arg;
}

void	Server::setRoot(const std::string &arg)
{
	_root = arg;
}

void	Server::setErrorLog(const std::string &arg)
{
	_error_log = arg;
}

void	Server::setErrorPage(const std::string &key, const std::string &value)
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

std::string	Server::getHost( void ) const
{
	return (_host);
}

std::string	Server::getPort( void ) const
{
	return (_port);
}

std::string	Server::getRoot( void ) const
{
	return (_root);
}

std::string	Server::getErrorLog( void ) const
{
	return (_error_log);
}

std::map<std::string, std::string>	Server::getErrorPages( void ) const
{
	return (_error_pages);
}

std::vector<std::string>	Server::getIndexes( void ) const
{
	return (_indexes);
}

std::vector<std::string>	Server::getServerNames( void ) const
{
	return (_server_names);
}

const std::vector<Location>&	Server::getLocations( void ) const
{
	return (_locations);
}



/* ************************************************************************** */