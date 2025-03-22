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

		ServerControler(); // set pfds_limit in constructor
		ServerControler( ServerControler const & src );
		~ServerControler();

		ServerControler&				operator=( ServerControler const & rhs );
		void							setServer( const Server & instance );
		const std::vector<Server>	&	getServers( void ) const;
		size_t							getServBlockNbr( void );
		Connection					*	getConnection(int fd);
		void							addConnection(int fd, int port, struct sockaddr_in & client);
		void							removeConnection(int fd);
		void							addPfd(int fd);
		void							removePfd(int fd);
		void							addCGIfd(int fd);
		void							removeCGIfd(int fd);
		void							setPfdEvent(int fd, char e);
		int								getNfds(void);
		int								incrementNfds(void);
		int								decrementNfds(void);
		void							closeFds(void);
		void							setWorkConnNum(size_t n);

		static void						sig_handler(int sig_num);

		void							startServing();

	private:

		unsigned int			_pfds_limit;
		unsigned int			_work_conn_num;

		std::vector<Server>		_servBlocks;
		std::vector<int>		_ports;
		std::vector<int>		_socketFds; // array of listening sockets identifiers (server_fds)
		std::vector<Connection*>	_conns;
		std::vector<int> 		_cgi_fds;
		struct pollfd 			_pfds[100000]; // size of pfds_limit
		int						_nfds;

		void	createListeningSockets(); // fill _socketFds
		void	polling();
		void	handleInEvent(int fd);
		void	handleOutEvent(int fd);
		int		isSocketFd(int fd);
		bool	isCGIfd(int fd);
		void	processRequest(Connection & conn);
		// std::string	processRequest(std::string & request, int port); // parse request and pass it to the rigth server block to get response
		Server & chooseServBlock(const std::string & host, int port);
};

std::ostream &			operator<<( std::ostream & o, ServerControler const & i );

#endif /* ****************************************************** SERVERCONTROLER_HPP */
