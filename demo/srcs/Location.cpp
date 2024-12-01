#include "../includes/Location.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Location::Location()
{
	this->_root = "var/www/html";
	this->_autoindex = false;
	this->_indexes.push_back("index.html");
	this->_client_max_body_size = 1;
}

/* Location::Location( const Location & src )
{
} */


/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Location::~Location()
{
}


/*
** --------------------------------- OVERLOAD ---------------------------------
*/

/* Location &				Location::operator=( Location const & rhs )
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