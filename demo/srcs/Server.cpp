#include "../includes/Server.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Server::Server()
	:	
		_location_nbr(0),
		_client_max_body_size(1 * 1024 * 1024), //1M
		_worker_connections(1024),
		_port(80),
		_host("localhost"),
		_root("./var/www/html"),
		_error_log("error.log")
{
	_indexes.push_back("index.html");
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
		this->_location_nbr = rhs.getLocationNbr();
		this->_client_max_body_size = rhs.getClientMaxBody();
		this->_worker_connections = rhs.getWorkCont();
		this->_host = rhs.getHost();
		this->_port = rhs.getPort();
		this->_root = rhs.getRoot();
		this->_error_log = rhs.getErrorLog();
		this->_error_pages = rhs.getErrorPages();
		this->_indexes = rhs.getIndexes();
		this->_server_names  = rhs.getServerNames();
		this->_locations = rhs.getLocations();
	}
	return *this;
}

std::ostream&			operator<<( std::ostream & o, Server const& i )
{
	o	<< "\n\t** Server **" << std::endl
		<<  "worker_connection: " << i.getWorkCont() << std::endl
		<<  "client_max_body_size: " << i.getClientMaxBody() << std::endl
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
		std::cout  << "\t" << intToStr(it->first) << ": " << it->second << std::endl;
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

/* bool find_file( const path & dir_path,         // in this directory,
                const std::string & file_name, // search for this name,
                path & path_found )            // placing path here if found
{
  if ( !exists( dir_path ) ) return false;
  directory_iterator end_itr; // default construction yields past-the-end
  for ( directory_iterator itr( dir_path );
        itr != end_itr;
        ++itr )
  {
    if ( is_directory(itr->status()) )
    {
      if ( find_file( itr->path(), file_name, path_found ) ) return true;
    }
    else if ( itr->leaf() == file_name ) // see below
    {
      path_found = itr->path();
      return true;
    }
  }
  return false;
} */

/** Append an HTTP rel-path to a local filesystem path.
 *  The created path is saved in a path parameter is normalized for the platform.
*/

int		Server::handleRequestedURI(std::string base_path, std::string& path)
{
	(void)path;
	if (base_path.empty())
	{
		base_path = "/";
	}
	path = "404.html";
	std::string root = this->getRoot();
	while (1)
	{
		for(int i = 0; i < this->_location_nbr; i++)
		{
			/** TODO:
			 * - find location which will start same
			 * - to ensure to check error_pages path for redirections
			 * Consider this example:

			root /var/www/main;

			location / {
				error_page 404 /another/whoops.html;
			}

			location /another {
				root /var/www;
			}

			Every request (other than those starting with /another) will be handled by the first block, which will serve files out of /var/www/main. However, if a file is not found (a 404 status),
			an internal redirect to /another/whoops.html will occur, leading to a new location search that will eventually land on the second block. This file will be served out of /var/www/another/whoops.html.
			*/
		}
	}

	return 0;
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

void	Server::setClientMaxBody(int value)
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

int Server::getWorkCont( void ) const
{
	return (_worker_connections);
}

int Server::getLocationNbr( void ) const
{
	return (_location_nbr);
}

int	Server::getClientMaxBody( void ) const
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


/* ************************************************************************** */