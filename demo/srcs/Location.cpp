#include "../includes/Location.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Location::Location() 
	:	_autoindex(false),
		_timeout(60),
		_client_max_body_size(1048576), // 1MB
		_name(""),
		_root("./var/www/html"),
		_upload_dir(""),
		_cgi_extension("")
{
	_indexes.push_back("index.html");
	_indexes.push_back("index.htm");
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