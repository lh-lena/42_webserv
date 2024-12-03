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
