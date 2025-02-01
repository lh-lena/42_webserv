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
# include "generate_html.hpp"

/**HttpStatusCode */
# define OK 200
# define CREATED 201
# define NO_CONTENT 204
# define MOVED_PERMANENTLY 301
# define FORBIDDEN 403
# define NOT_FOUND 404
# define CONFLICT 409
# define METHOD_NOT_ALLOWED 405
# define NOT_IMPLEMENTED 501
# define INTERNAL_SERVER_ERROR 500

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
		std::string							getCustomErrorPage(int status_code, const Location& src);
		void								handleStaticRequest(const Request& request, Response& response, const Location& loc);
		bool								findRequestedLocation(const std::string& path, Location& loc);
		// int									handleRequestedURI(std::string requested_path, std::string& path, Location& loc, bool& location_found);
		void								handleRequestedURI(Response& response, const Location& loc);
		bool								searchingPrefixMatchLocation(std::string requested_path, Location& location);
		bool								searchingExtensionMatchLocation(std::string requested_path, Location& location);
		int									searchingUploadDir(std::string requested_path, std::string& path, const Location& location, bool& location_found);
		std::string							determineFilePath(std::string requested_path, const Location& loc);	
		bool								appendIndexFile(std::string& path, const Location& loc);
		void								handleGET(Response& response, const Location& loc);
		void								handlePOST(const Request& request, Response& response, const Location& loc);
		void								handleDELETE(Response& response, const Location& loc);
		void								setGetResponse(Response& response, size_t status_code);
		void								setErrorResponse(Response& response, size_t status_code);
		void								setPostResponse(Response& response, size_t status_code);
		void								setDeleteResponse(Response& response, size_t status_code);
		void								handleMethodNotAllowed(Response& response, const Location& location);
		void								handleAndSetRedirectResponse(Response& response, const Location& loc);
		void								handleGetDirectoryResponse(Response& response, const Location& loc);
		void								createResponse(const Response& response, std::string& result);
		void								initResponse(Response& response, const Request& request);
		size_t								handleDeleteDirectoryResponse(Response& response);
		size_t								remove_file(const std::string& path);
		size_t								remove_directory(const std::string& path);
		std::string							formatDate(time_t timestamp);
		int									handleCGI(Response& response, Location& loc);
		void								setCGIResponse(Response& response, size_t status_code);

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
