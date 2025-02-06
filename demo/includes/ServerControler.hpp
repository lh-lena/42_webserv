#ifndef SERVERCONTROLER_HPP
# define SERVERCONTROLER_HPP

# include "Server.hpp"
# include "Connection.hpp"
# include "Request.hpp"
# include <string>
# include <vector>
# include <iostream>
# include <sys/poll.h>
# include <stdexcept>
# include <sys/resource.h>

# define MAX_CONN_NUM 1024

extern volatile bool g_serv_end;

class ServerControler
{

	public:

		ServerControler();
		ServerControler( ServerControler const & src );
		~ServerControler();

		ServerControler&				operator=( ServerControler const & rhs );
		void							setServer( const Server& instance );
		const std::vector<Server>	&	getServers( void ) const;
		size_t							getServBlockNbr( void );
		Connection					&	getConnection(int fd);
		void							addConnection(int fd);
		void							removeConnection(int fd);
		void							addPfd(int fd);
		void							removePfd(int fd);
		int								getNfds(void);
		int								incrementNfds(void);
		int								decrementNfds(void);

		static void						sig_handler(int sig_num);

		void							startServing();
		//bool	servEnd;

		unsigned int				pfds_limit;

	private:

		std::vector<Server>	_servBlocks;
		std::vector<int>	_socketFds; // array of listening sockets identifiers (server_fds)
		std::vector<Connection>	_conns;
		struct pollfd _pfds[MAX_CONN_NUM];
		int	_nfds;

		void	createListeningSockets(); // fill _socketFds
		std::string	processRequest(std::string & request); // parse request and pass it to the rigth server block to get response
		Server & chooseServBlock(std::string & host);
};

std::ostream &			operator<<( std::ostream & o, ServerControler const & i );

#endif /* ****************************************************** SERVERCONTROLER_HPP */
