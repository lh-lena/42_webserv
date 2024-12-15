#include "../includes/HttpServer.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

HttpServer::HttpServer()
{
}

HttpServer::HttpServer( const HttpServer& src )
{
	*this = src;
}


/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

HttpServer::~HttpServer()
{
}


/*
** --------------------------------- OVERLOAD ---------------------------------
*/

HttpServer&				HttpServer::operator=( HttpServer const& rhs )
{
	if ( this != &rhs )
	{
		this->_server = rhs._server;
	}
	return *this;
}

std::ostream&			operator<<( std::ostream & o, HttpServer const& i )
{
	std::vector<Server> servers = i.getServers();
	for (Server server : servers)
	{
    	o << server << std::endl;
	}
	return o;
}


/*
** --------------------------------- METHODS ----------------------------------
*/


/*
** --------------------------------- ACCESSOR ---------------------------------
*/

void	HttpServer::setServer(Server instance)
{
	_server.push_back(instance);
}

const std::vector<Server>&	HttpServer::getServers( void ) const
{
	return (_server);
}

/* ************************************************************************** */