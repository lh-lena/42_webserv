#ifndef SERVER_HPP
# define SERVER_HPP

# include <map>
# include <ctime>
# include <string>
# include <vector>
# include <cstdio>
# include <unistd.h>
# include <iostream>
# include <algorithm>
# include "Location.hpp"
# include "Request.hpp"
# include "Response.hpp"

class Server
{
	public:
		Server();
		Server( Server const & src );
		Server &		operator=( Server const & rhs );
		~Server();

		void								defaultServer( void );
		void								addLocationNbr(int value);
		void								setClientMaxBody(int value);
		void								setWorkCont(int connections);
		void								setHost(const std::string &arg);
		void								setPort( int val );
		void								setRoot(const std::string &arg);
		void								setErrorLog(const std::string &arg);
		void								addErrorPage(int key, const std::string &value);
		void								setIndex(const std::string &arg);
		void								setServerName(const std::string &arg);
		void								setLocation(const Location &src);
		void								setImplementedMethods(const std::string& str);
		int									getLocationNbr( void ) const;
		int									getClientMaxBody( void ) const;
		int									getWorkCont( void ) const;
		int									getPort( void ) const;
		const std::string&					getHost( void ) const;
		const std::string&					getRoot( void ) const;
		const std::string&					getErrorLog( void ) const;
		const std::vector<Location>&		getLocations( void ) const;
		const std::vector<std::string>&		getIndexes( void ) const;
		const std::vector<std::string>&		getServerNames( void ) const;
		const std::map<int, std::string>&	getErrorPages( void ) const;
		const std::vector<std::string>&		getImplementedMethods( void ) const;

		int									handleCGI(Response& response, Location& loc);

		std::string							server_name;

	private:
		int									_location_nbr;
		int									_client_max_body_size;
		int									_worker_connections;
		int									_port;
		std::string							_host;
		std::string							_root;
		std::string							_error_log;
		std::vector<Location>				_locations;
		std::vector<std::string>			_indexes;
		std::vector<std::string>			_implemented_methods;
		std::vector<std::string>			_server_names;
		std::map<int, std::string>			_error_pages;
};

std::ostream &			operator<<( std::ostream & o, Server const & i );

#endif /* ******************************************************** SERVER_HPP */
