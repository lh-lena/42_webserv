#include "../includes/Location.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Location::Location() 
	:	_root("var/www/html"),
		_autoindex(false),
		_client_max_body_size(1)
{
	_indexes.push_back("index.html");
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