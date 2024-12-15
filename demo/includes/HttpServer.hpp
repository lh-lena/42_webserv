#ifndef HTTPSERVER_HPP
# define HTTPSERVER_HPP

# include "Server.hpp"
# include <string>
# include <vector>
# include <iostream>


class HttpServer
{

	public:

		HttpServer();
		HttpServer( HttpServer const & src );
		~HttpServer();

		HttpServer&					operator=( HttpServer const & rhs );
		void						setServer(Server instance);
		const std::vector<Server>&	getServers( void ) const;

	private:
		std::vector<Server>		_server;

};

std::ostream &			operator<<( std::ostream & o, HttpServer const & i );

#endif /* ****************************************************** HTTPSERVER_HPP */