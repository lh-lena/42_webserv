#include "../includes/Location.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Location::Location()
	:	_autoindex(false),
		_client_max_body_size(0)
{
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
		this->_alias = rhs.getAlias();
		this->_return = rhs.getRedirect();
		this->_indexes = rhs.getIndexes();
		this->_autoindex = rhs.getAutoindex();
		this->_upload_dir = rhs.getUploadDir();
		this->_error_pages = rhs.getErrorPages();
		this->_cgi_extension = rhs.getCGIExtension();
		this->_method = rhs.getAllowedMethods();
	}
	return *this;
}

std::ostream&			operator<<( std::ostream & o, Location const& i )
{
	o	<< "\n\t** Location **" << std::endl
		<< "path: " << i.getPath() << std::endl
		<< "root: " << i.getRoot() << std::endl
		<< "alias: " << i.getAlias() << std::endl
		<< "autoindex: " << i.getAutoindex() << std::endl
		<< "clien max body: " << i.getClientMaxBody() << std::endl
		<< "upload dir: " << i.getUploadDir() << std::endl;

	o	<< "indexes: ";
	std::vector<std::string>::const_iterator it_s;
	for (it_s = i.getIndexes().begin(); it_s != i.getIndexes().end(); ++it_s)
	{
		o << *it_s << " ";
	}
	o << "\n";

	o << "CGI extention: ";
	for (it_s = i.getCGIExtension().begin(); it_s != i.getCGIExtension().end(); ++it_s)
	{
		o << *it_s << " ";
	}
	o << "\n";

	o	<< "allowed methods: ";
	for (it_s = i.getAllowedMethods().begin(); it_s != i.getAllowedMethods().end(); ++it_s)
	{
		o << *it_s << " ";
	}
	o << "\n";

	o	<< "error pages: ";

	std::map<int, std::string>::const_iterator it;
	for (it = i.getErrorPages().begin(); it != i.getErrorPages().end(); ++it)
	{
		std::cout  << "\t" << utils::itos(it->first) << ": " << it->second << std::endl;
	}
	o << "\n";
	o	<< "redirection: ";
	for (it = i.getRedirect().begin(); it != i.getRedirect().end(); ++it)
	{
		std::cout  << "\t" << utils::itos(it->first) << ": " << it->second << std::endl;
	}
	o << "\n";
	return o;
}

/*
** --------------------------------- METHODS ----------------------------------
*/

void	Location::defaultRootLocation( void )
{
	setPath("/");
	setRoot("var/www/html");
	setIndex("index.42webserver.html");
}

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

void	Location::setAlias(const std::string &arg)
{
	_alias = arg;
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
	_method.push_back(arg);
}

void	Location::setUploadDir(const std::string &arg)
{
	_upload_dir = arg;
}

void	Location::addErrorPage(int	key, const std::string &val)
{
	_error_pages[key] = val;
}

void	Location::setRedirect( int key, const std::string &val ) 
{
	_return[key] = val;
}

void	Location::setCGIExtension( const std::string &arg )
{
	_cgi_extension.push_back(arg);
}

void	Location::setCgiInterpreter( const std::string &key, const std::string& val)
{
	_cgi_interpreter[key] = val;
}

void		Location::setClientMaxBody(size_t value)
{
	_client_max_body_size = value;
}

size_t			Location::getClientMaxBody( void ) const
{
	return(_client_max_body_size);
}

const std::string&		Location::getPath( void ) const
{
	return (_path);
}

const std::string&		Location::getRoot( void ) const
{
	return (_root);
}

const std::string&		Location::getAlias( void ) const
{
	return (_alias);
}

const std::vector<std::string>&	Location::getIndexes( void ) const
{
	return (_indexes);
}

bool	Location::getAutoindex( void ) const
{
	return (_autoindex);
}

const std::vector<std::string>&	Location::getAllowedMethods( void ) const
{
	return (_method);
}

const std::string&	Location::getUploadDir( void ) const
{
	return (_upload_dir);
}


const std::map<int, std::string>&	Location::getErrorPages( void ) const
{
	return (_error_pages);
}

const std::map<int, std::string>&	Location::getRedirect( void ) const
{
	return (_return);
}

const std::vector<std::string>&		Location::getCGIExtension( void ) const
{
	return (_cgi_extension);
}

const std::map<std::string, std::string>&		Location::getCgiInterpreter( void ) const
{
	return (_cgi_interpreter);
}

/* ************************************************************************** */