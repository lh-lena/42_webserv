#include "../includes/Server.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Server::Server()
{
	this->_host = "localhost";
	this->_root = "var/www/html";
	this->_indexes.push_back("index.html");
	this->_client_max_body_size = 1;
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


/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */