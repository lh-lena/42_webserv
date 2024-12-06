#include "HttpServer.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

HttpServer::HttpServer()
{
}

HttpServer::HttpServer( const HttpServer & src )
{
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

HttpServer &				HttpServer::operator=( HttpServer const & rhs )
{
	//if ( this != &rhs )
	//{
		//this->_value = rhs.getValue();
	//}
	return *this;
}

std::ostream &			operator<<( std::ostream & o, HttpServer const & i )
{
	//o << "Value = " << i.getValue();
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

std::vector<Server>	HttpServer::getServer( void )
{
	return (_server);
}

/* ************************************************************************** */