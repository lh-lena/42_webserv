#ifndef SERVERCONTROLER_HPP
# define SERVERCONTROLER_HPP

# include "Server.hpp"
# include <string>
# include <vector>
# include <iostream>

extern volatile bool g_serv_end;

class ServerControler
{

	public:

		ServerControler();
		ServerControler( ServerControler const & src );
		~ServerControler();

		ServerControler&					operator=( ServerControler const & rhs );
		void								setServer( const Server& instance );
		const std::vector<Server>&			getServers( void ) const;
		size_t								getServBlockNbr( void );
		static void							sig_handler(int sig_num);

		void							startServing();
		bool	servEnd;

	private:
		std::vector<Server>					_servBlocks;
		std::vector<int>	_socketFds; // array of listening sockets identifiers (server_fds)

		void	createListeningSockets(); // fill _socketFds
		std::string	processRequest(std::string & request); // parse request and pass it to the rigth server block to get response
		Server & chooseServBlock(std::string & host);
};

std::ostream &			operator<<( std::ostream & o, ServerControler const & i );

#endif /* ****************************************************** SERVERCONTROLER_HPP */
