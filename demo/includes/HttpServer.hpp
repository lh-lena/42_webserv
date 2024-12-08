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

		HttpServer &			operator=( HttpServer const & rhs );
		void					setServer(Server instance);
		std::vector<Server>		getServer( void );

	private:
		std::vector<Server>		_server;

};

// std::ostream &			operator<<( std::ostream & o, HttpServer const & i );

#endif /* ****************************************************** HTTPSERVER_H */