#include "../includes/Server.hpp"
#include "../includes/Response.hpp"
#include "../includes/Request.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Server::Server()
	:
		server_name("Webserv/1.0"),
		_client_max_body_size(1 * 1024 * 1024), //1M
		_location_nbr(0),
		_worker_connections(1024),
		_port(0)
{
	setImplementedMethods("GET");
	setImplementedMethods("HEAD");
	setImplementedMethods("POST");
	setImplementedMethods("DELETE");
}

Server::Server( const Server & src )
{
	*this = src;
}


/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Server::~Server()
{
}


/*
** --------------------------------- OVERLOAD ---------------------------------
*/

Server &				Server::operator=( Server const & rhs )
{
	if ( this != &rhs )
	{
		_location_nbr = rhs.getLocationNbr();
		_client_max_body_size = rhs.getClientMaxBody();
		_worker_connections = rhs.getWorkCont();
		_host = rhs.getHost();
		_port = rhs.getPort();
		_root = rhs.getRoot();
		_upload_dir = rhs.getUploadDir();
		_error_log = rhs.getErrorLog();
		_error_pages = rhs.getErrorPages();
		_indexes = rhs.getIndexes();
		_server_names  = rhs.getServerNames();
		_locations = rhs.getLocations();
		_implemented_methods = rhs.getImplementedMethods();
		server_name = rhs.server_name;
	}
	return *this;
}

std::ostream&			operator<<( std::ostream & o, Server const& i )
{
	o	<< "\n\t** Server **" << std::endl
		<<  "worker connection: " << i.getWorkCont() << std::endl
		<<  "client max body size: " << i.getClientMaxBody() << std::endl
		<< "host: " << i.getHost() << std::endl
		<< "port: " << i.getPort() << std::endl
		<< "root: " << i.getRoot() << std::endl;
	o	<< "indexes: \n\t";
	std::vector<std::string>::const_iterator it_s;
	for (it_s = i.getIndexes().begin(); it_s != i.getIndexes().end(); ++it_s)
	{
		o << *it_s << " ";
	}
	o	<< "\nserver names: \n\t";
	for (it_s = i.getServerNames().begin(); it_s != i.getServerNames().end(); ++it_s)
	{
		o << *it_s << " ";
	}
	o	<< "\nerror pages: \n";
	std::map<int, std::string>::const_iterator it;
	for (it = i.getErrorPages().begin(); it != i.getErrorPages().end(); ++it)
	{
		std::cout  << "\t" << utils::itos(it->first) << ": " << it->second << std::endl;
	}
	o << std::endl;

	std::vector<Location> locations = i.getLocations();
	o << "Locations:  " << locations.size() << std::endl;
	for (size_t i = 0; i < locations.size(); i++)
	{
		o << locations[i] << std::endl;
	}

	return o;
}

/*
** --------------------------------- METHODS ----------------------------------
*/

void		Server::defaultServer( void )
{
	setHost("127.0.0.1");
	setPort(8080);
	setRoot("var/www/html");
	setIndex("index.42webserver.html");
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

void Server::setWorkCont(int connections)
{
	_worker_connections = connections;
}

void Server::addLocationNbr(int value)
{
	_location_nbr += value;
}

void	Server::setClientMaxBody(size_t value)
{
	_client_max_body_size = value;
}

void	Server::setHost(const std::string &arg)
{
	_host = arg;
}

void	Server::setPort( int val )
{
	_port = val;
}

void	Server::setRoot(const std::string &arg)
{
	_root = arg;
}

void	Server::setErrorLog(const std::string &arg)
{
	_error_log = arg;
}

void	Server::addErrorPage(int key, const std::string &value)
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

void	Server::setLocation(const Location &src)
{
	_locations.push_back(src);
	_location_nbr += 1;
}

void	Server::setUploadDir(const std::string &str)
{
	_upload_dir = str;
}

void	Server::setImplementedMethods(const std::string &str)
{
	_implemented_methods.push_back(str);
}

void	Server::setCGIExtension( const std::string &arg )
{
	_cgi_extension.push_back(arg);
}

void	Server::setCgiInterpreter( const std::string &key, const std::string& val)
{
	_cgi_interpreter[key] = val;
}

int Server::getWorkCont( void ) const
{
	return (_worker_connections);
}

int Server::getLocationNbr( void ) const
{
	return (_location_nbr);
}

size_t	Server::getClientMaxBody( void ) const
{
	return (_client_max_body_size);
}

const std::string&	Server::getHost( void ) const
{
	return (_host);
}

int		Server::getPort( void ) const
{
	return (_port);
}

const std::string&	Server::getRoot( void ) const
{
	return (_root);
}

const std::string&	Server::getErrorLog( void ) const
{
	return (_error_log);
}

const std::string&	Server::getUploadDir( void ) const
{
	return (_upload_dir);
}

const std::map<int, std::string>&	Server::getErrorPages( void ) const
{
	return (_error_pages);
}

const std::vector<std::string>&	Server::getIndexes( void ) const
{
	return (_indexes);
}

const std::vector<std::string>&	Server::getServerNames( void ) const
{
	return (_server_names);
}

const std::vector<Location>&	Server::getLocations( void ) const
{
	return (_locations);
}

const std::vector<std::string>&	Server::getImplementedMethods( void ) const
{
	return (_implemented_methods);
}


const std::map<std::string, std::string>&	Server::getCgiInterpreter( void ) const
{
	return (_cgi_interpreter);
}

const std::vector<std::string>&		Server::getCGIExtension( void ) const
{
	return (_cgi_extension);
}

/* ************************************************************************** */
