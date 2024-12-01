#ifndef SERVER_HPP
# define SERVER_HPP

# include <map>
# include <string>
# include <vector>
# include <iostream>

class Server
{
	public:
		Server();
		// Server( Server const & src );
		~Server();
		// Server &		operator=( Server const & rhs );

	private:
		int									_location_nbr;
		std::string							_host;
		std::string							_port;
		std::string							_root;
		std::string							_access_log;
		std::string							_client_max_body_size;
		std::vector<std::string>			_indexes;
		std::vector<std::string>			_server_names;
		std::map<std::string, std::string>	_error_pages;
};

#endif /* ******************************************************** SERVER_HPP */
