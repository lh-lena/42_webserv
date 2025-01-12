#ifndef SERVER_HPP
# define SERVER_HPP

# include <map>
# include <string>
# include <vector>
# include <iostream>
# include <algorithm>
# include "Location.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include "generate_html.hpp"

class Server
{
	public:
		Server();
		Server( Server const & src );
		Server &		operator=( Server const & rhs );
		~Server();

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
		std::string							handleErrorPageResponse(int status_code, const Location& src);
		void								handleRequestMethod(const Request& request, Response& response);
		// int									handleRequestedURI(std::string requested_path, std::string& path, Location& loc, bool& location_found);
		int									handleRequestedURI(Response& response, Location& loc);
		int									prefixMatchURI(std::string requested_path, std::string& path, Location& location, bool& location_found);
		bool								appendIndexFile(std::string& path, const Location& loc);
		void								handleGET(Response& response, Location& location);
		void								handlePOST();
		void								handleDELETE();
		void								handleUnsupportedMethod(Response& response);

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
		std::vector<std::string>			_server_names;
		std::map<int, std::string>			_error_pages;
};

std::ostream &			operator<<( std::ostream & o, Server const & i );

#endif /* ******************************************************** SERVER_HPP */
