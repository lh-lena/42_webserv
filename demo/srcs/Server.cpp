#include "../includes/Server.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Server::Server()
	:	
		_location_nbr(0),
		_client_max_body_size(1048576), //1M
		_worker_connections(1024),
		_host("localhost"),
		_port("8080"),
		_root("./var/www/html"),
		_error_log("error.log")
{
	_indexes.push_back("index.html");
	_indexes.push_back("index.htm");
}

/* Server::Server( const Server & src )
{
} */


/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Server::~Server()
{
}


/*
** --------------------------------- OVERLOAD ---------------------------------
*/

/* Server &				Server::operator=( Server const & rhs )
{
	//if ( this != &rhs )
	//{
		//this->_value = rhs.getValue();
	//}
	return *this;
} */


/*
** --------------------------------- METHODS ----------------------------------
*/

void	Location::setRoot(const std::string &arg)
{
	_root = arg;
}

std::string	Location::getRoot( void )
{
	return (_root);
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

int Server::getWorkCont( void )
{
	return (_worker_connections);
}

int Server::getLocationNbr( void )
{
	return (_location_nbr);
}

int	Server::getClientMaxBody( void )
{
	return (_client_max_body_size);
}

std::string	Server::getHost( void )
{
	return (_host);
}

std::string	Server::getRoot( void )
{
	return (_root);
}

std::string	Server::getErrorLog( void )
{
	return (_error_log);
}

std::string	Server::getErrorPage( const std::string &key, const std::string &value )
{
	return (_error_pages[key]);
}

std::vector<std::string>	Server::getIndex( void )
{
	return (_indexes);
}

std::vector<std::string>	Server::getServerName( void )
{
	return (_server_names);
}


void Server::setWorkCont(int connections)
{
	_worker_connections = connections;
}

void Server::setLocationNbr(int value)
{
	_location_nbr = value;
}

void	Server::setClientMaxBody(int value)
{
	_client_max_body_size = value;
}

void	Server::setHost(const std::string &arg)
{
	_host = arg;
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

void	Server::setLocation(Location &src)
{
	_locations.push_back(src);
}


/* ************************************************************************** */