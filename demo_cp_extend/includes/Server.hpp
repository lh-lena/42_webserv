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
		Server( Server const & src );
		Server &		operator=( Server const & rhs );
		~Server();

		void								setLocationNbr(int value);
		void								setClientMaxBody(int value);
		void								setWorkCont(int connections);
		void								setHost(const std::string &arg);
		void								setPort(const std::string &arg);
		void								setRoot(const std::string &arg);
		void								setErrorLog(const std::string &arg);
		void								setErrorPage(const std::string &key, const std::string &value);
		void								setIndex(const std::string &arg);
		void								setServerName(const std::string &arg);
		void								setLocation(const Location &src);
		int									getLocationNbr( void ) const;
		int									getClientMaxBody( void ) const;
		int									getWorkCont( void )  const;
		std::string							getHost( void ) const;
		std::string							getPort( void ) const;
		std::string							getRoot( void ) const;
		std::string							getErrorLog( void ) const;
		std::vector<Location>				getLocations( void ) const;
		std::vector<std::string>			getIndexes( void ) const;
		std::vector<std::string>			getServerNames( void ) const;
		std::map<std::string, std::string>	getErrorPages( void ) const;

		void	generateResponse(); // TODO: decide in what form response is returned

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

		void	copyFileToBuf(char *buf, std::string & file);
		void	runCGI(std::string & file);
		//std::string	findFile(std::string path); // check the requested is available in given locations

};

std::ostream &			operator<<( std::ostream & o, Server const & i );

#endif /* ******************************************************** SERVER_HPP */
