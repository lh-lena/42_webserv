#ifndef SERVERCONTROLER_HPP
# define SERVERCONTROLER_HPP

# include "Server.hpp"
# include <string>
# include <vector>
# include <iostream>


class ServerControler
{

	public:

		ServerControler();
		ServerControler( ServerControler const & src );
		~ServerControler();

		ServerControler&					operator=( ServerControler const & rhs );
		void								setServer(Server instance);
		const std::vector<Server>&			getServers( void ) const;

	private:
		std::vector<Server>					_server;

};

std::ostream &			operator<<( std::ostream & o, ServerControler const & i );

#endif /* ****************************************************** SERVERCONTROLER_HPP */