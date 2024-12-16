#include "../includes/Location.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Location::Location() 
	:	_autoindex(false),
		_timeout(60),
		_client_max_body_size(1048576), // 1MB
		_path(""),
		_root("./var/www/html"),
		_upload_dir(""),
		_cgi_extension("")
{
	// _indexes.push_back("index.html");
	// _indexes.push_back("index.htm");
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

void	Location::setPath(const std::string &arg)
{
	_path = arg;
}

void	Location::setRoot(const std::string &arg)
{
	_root = arg;
}

void	Location::setIndex(const std::string &arg)
{
	_indexes.push_back(arg);
}

void	Location::setAutoindex(bool arg)
{
	_autoindex = arg;
}

void	Location::setAllowedMethod(const std::string &arg)
{
	_allowed_methods.push_back(arg);
}

void	Location::setClientMaxBody(int arg)
{
	_client_max_body_size = arg;
}

void	Location::setUploadDir(const std::string &arg)
{
	_upload_dir = arg;
}

std::string	Location::getPath( void ) const
{
	return (_path);
}

std::string	Location::getRoot( void ) const
{
	return (_root);
}

std::vector<std::string>	Location::getIndexes( void ) const
{
	return (_indexes);
}

bool	Location::getAutoindex( void ) const
{
	return (_autoindex);
}

std::vector<std::string>	Location::getAllowedMethods( void ) const
{
	return (_allowed_methods);
}

int	Location::getClientMaxBody( void ) const
{
	return (_client_max_body_size);
}

std::string	Location::getUploadDir( void ) const
{
	return (_upload_dir);
}

/* ************************************************************************** */