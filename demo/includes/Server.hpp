#ifndef SERVER_HPP
# define SERVER_HPP

# include <map>
# include <string>
# include <vector>
# include <iostream>
# include "Location.hpp"

class Server
{
	public:
		Server();
		// Server( Server const & src );
		~Server();
		// Server &		operator=( Server const & rhs );

		void								setWorkCont(int connections);
		void								setLocationNbr(int value);
		void								setClientMaxBody(int value);
		void								setHost(const std::string &arg);
		void								setRoot(const std::string &arg);
		void								setErrorLog(const std::string &arg);
		void								setErrorPage(const std::string &key, const std::string &value);
		void								setIndex(const std::string &arg);
		void								setServerName(const std::string &arg);
		void								setLocation(Location &src);
		int									getWorkCont( void );
		int									getLocationNbr( void );
		int									getClientMaxBody( void );
		std::string							getHost( void );
		std::string							getRoot( void );
		std::string							getErrorLog( void );
		std::string							getErrorPage( const std::string &key, const std::string &value );
		std::vector<std::string>			getIndex( void );
		std::vector<std::string>			getServerName( void );
		// std::vector<Location>			getLocation( void );

	private:
		int									_location_nbr;
		int									_client_max_body_size;
		int									_worker_connections;
		std::string							_host;
		std::string							_port;
		std::string							_root;
		std::string							_error_log;
		std::vector<Location>				_locations;
		std::vector<std::string>			_indexes;
		std::vector<std::string>			_server_names;
		std::map<std::string, std::string>	_error_pages;
};

#endif /* ******************************************************** SERVER_HPP */
