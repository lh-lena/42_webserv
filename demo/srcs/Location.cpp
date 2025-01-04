#include "../includes/Location.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Location::Location() 
	:	_autoindex(false),
		_root("./var/www/html")
{
	// _indexes.push_back("index.html");
	// _indexes.push_back("index.htm");
}

Location::Location( const Location & src )
{
	*this = src;
}


/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Location::~Location()
{
}


/*
** --------------------------------- OVERLOAD ---------------------------------
*/

Location &				Location::operator=( Location const & rhs )
{
	if ( this != &rhs )
	{
		this->_root = rhs.getRoot();
		this->_path = rhs.getPath();
		this->_return = rhs.getReturn();
		this->_indexes = rhs.getIndexes();
		this->_autoindex = rhs.getAutoindex();
		this->_upload_dir = rhs.getUploadDir();
		this->_error_pages = rhs.getErrorPages();
		this->_cgi_extension = rhs.getCgiExtension();
		this->_allowed_methods = rhs.getAllowedMethods();
	}
	return *this;
}

std::ostream&			operator<<( std::ostream & o, Location const& i )
{
	o	<< "\n\t** Location **" << std::endl
		<< "path: " << i.getPath() << std::endl
		<< "root: " << i.getRoot() << std::endl
		<< "autoindex: " << i.getAutoindex() << std::endl
		<< "CGI extention: " << i.getCgiExtension() << std::endl
		<< "upload dir: " << i.getUploadDir() << std::endl;

	o	<< "indexes: \n\t";
	std::vector<std::string>::iterator it_s;
	for (it_s = i.getIndexes().begin(); it_s != i.getIndexes().end(); ++it_s)
	{
		o << *it_s << " ";
	}
	o	<< "\nallowed methods: \n\t";
	for (it_s = i.getAllowedMethods().begin(); it_s != i.getAllowedMethods().end(); ++it_s)
	{
		o << *it_s << " ";
	}
	o	<< "\nerror pages: \n";

	std::map<int, std::string>::iterator it;
	for (it = i.getErrorPages().begin(); it != i.getErrorPages().end(); ++it)
	{
		std::cout  << "\t" << intToStr(it->first) << ": " << it->second << std::endl;
	}
	o	<< "\nredirection: \n";
	for (it = i.getReturn().begin(); it != i.getReturn().end(); ++it)
	{
		std::cout  << "\t" << intToStr(it->first) << ": " << it->second << std::endl;
	}
	o << std::endl;
	return o;
}

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

void	Location::setUploadDir(const std::string &arg)
{
	_upload_dir = arg;
}

void	Location::addErrorPage(int	key, const std::string &val)
{
	_error_pages[key] = val;
}

void	Location::setReturn( int key, const std::string &val ) 
{
	_return[key] = val;
}

void	Location::setCgiExtension( const std::string &arg )
{
	_cgi_extension = arg;
}

std::string		Location::getPath( void ) const
{
	return (_path);
}

std::string		Location::getRoot( void ) const
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

std::string	Location::getUploadDir( void ) const
{
	return (_upload_dir);
}


std::map<int, std::string>	Location::getErrorPages( void ) const
{
	return (_error_pages);
}

std::map<int, std::string>	Location::getReturn( void ) const
{
	return (_return);
}

std::string		Location::getCgiExtension( void ) const
{
	return (_cgi_extension);
}

/* ************************************************************************** */