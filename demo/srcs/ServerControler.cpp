#include "../includes/ServerControler.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstdlib>
#include <fcntl.h>
#include <cstring>
#include <vector>
#include <stdexcept>
#include <csignal>

volatile bool g_serv_end = false;

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

ServerControler::ServerControler(): servEnd(false)
{
}

ServerControler::ServerControler( const ServerControler& src )
{
	*this = src;
}


/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

ServerControler::~ServerControler()
{
}


/*
** --------------------------------- OVERLOAD ---------------------------------
*/

ServerControler&				ServerControler::operator=( ServerControler const& rhs )
{
	if ( this != &rhs )
	{
		this->_servBlocks = rhs._servBlocks;
	}
	return *this;
}

std::ostream&			operator<<( std::ostream & o, ServerControler const& i )
{
	std::vector<Server> servers = i.getServers();
	for (size_t i = 0; i < servers.size(); i++)
	{
		o << servers[i] << std::endl;
	}
	return o;
}

/*
** --------------------------------- STATIC -----------------------------------
*/
static int	set_to_nonblocking(int server_fd)
{
	// Set server socket to non-blocking
	int flags = fcntl(server_fd, F_GETFL, 0);
	if (flags == -1)
	{
		throw std::runtime_error(strerror(errno));
		// std::cerr << "Error: " << strerror(errno) << std::endl;
		// close(server_fd);
		// return -6;
	}
	int res = fcntl(server_fd, F_SETFL, flags | O_NONBLOCK);
	if (res == -1)
	{
		throw std::runtime_error(strerror(errno));
		// std::cerr << "Error: " << strerror(errno) << std::endl;
		// close(server_fd);
		// return -7;
	}
	return 1;
}

static int	create_tcp_server_socket(int port)
{
	int server_fd;
	int reuse = 1; // Allow socket address reuse
	struct sockaddr_in socket_addr; // represents IPv4 socket addresses

	/* Step1: create a TCP socket */
	server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server_fd == -1)
	{
		throw std::runtime_error(strerror(errno));
		// std::cerr << "Error: creation socket failed: " << strerror(errno) << std::endl;
		// return -2;
	}

	/*This helps in manipulating options for the socket referred by the file descriptor sockfd. Optional*/
	int res = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char *) &reuse, sizeof(reuse));

	if (res == -1)
	{
		throw std::runtime_error("Error: set socket options failed");
		// std::cerr << "Error: set socket options failed" << std::endl;
		// close(server_fd);
		// return -3;
	}

	/* Initialize the socket address structure */
	// bind the socket to a IP / port
	memset(&socket_addr, 0, sizeof(socket_addr));
	socket_addr.sin_family = AF_INET;
	socket_addr.sin_port = htons(port);
	socket_addr.sin_addr.s_addr = INADDR_ANY; // Bind to any available interface
	/*
	//ip address of www.msn.com (get by doing a ping www.msn.com at terminal)
	server.sin_addr.s_addr = inet_addr("131.253.13.140");//INADDR_ANY
	if (server.sin_addr.s_addr != INADDR_NONE) // good
	*/
	set_to_nonblocking(server_fd);

	// associates the socket with a specific IP address and port number
	if (bind(server_fd, (sockaddr*)&socket_addr, sizeof(sockaddr)) == -1)
	{
		throw std::runtime_error(strerror(errno));
		// std::cerr << "Error: binding socket failed: " << strerror(errno) << std::endl;
		// close(server_fd);
		// return -4;
	}

	// Listen for connections. Max connections is 5 LISTEN_BACKLOG
	if (listen(server_fd, SOMAXCONN) == -1)
	{
		throw std::runtime_error(strerror(errno));
		// std::cerr << "Error listening on socket: " << strerror(errno) << std::endl;
		// close(server_fd);
		// return -5;
	}

	//std::cout << "Server listening on port " << port << std::endl;
	return (server_fd);
}


/*
** --------------------------------- METHODS ----------------------------------
*/
static int	isInPollfds(int fd, const std::vector<int> & sds)
{
	int	size = sds.size();
	for (int i = 0; i < size; i++)
	{
		if (fd == sds[i])
			return sds[i];
	}
	return 0;
}

void	ServerControler::startServing()
{
	int timeout, size, res;
	std::vector<std::string> bufs;
	struct pollfd pfds[200]; //max number of connections
	int nfds = 0;
	int new_fd = -1;
	// bool conn_active = false;
	char buf[1500];

	try
	{
		this->createListeningSockets();
	}
	catch(const std::exception& e)
	{
		// close all sockets;
		std::cerr << e.what() << '\n';
		throw std::runtime_error("Error: socket setup failed");
	}

	memset(pfds, 0, sizeof(pfds));
	size = _socketFds.size();
	for (int i = 0; i < size; i++)
	{
		pfds[i].fd = _socketFds[i];
		pfds[i].events = POLLIN;
		nfds++;
	}

	timeout = 1 * 60 * 1000;
	//servEnd = true;
	while (!servEnd)
	{
		std::cout << "Poll is started" << std::endl;
		res = poll(pfds, nfds, timeout);
		if (res < 0)
		{
			throw std::runtime_error(strerror(errno));
			//close(_socketFds);
			// std::cerr << "Error on poll: " << strerror(errno) << std::endl;
			// return;
		}
		if (res == 0)
		{
			std::cout << "Timeout" << std::endl;
			//close(_socketFds);
			return;
		}
		size = nfds;
		for (int i = 0; i < size; i++)
		{
			if (pfds[i].revents == POLLIN)
			{
				res = isInPollfds(pfds[i].fd, _socketFds);
				if (res)
				{
					do
					{
						new_fd = accept(res, NULL, NULL);
						if (new_fd < 0 && errno != EWOULDBLOCK)
							throw std::runtime_error("Error: accept() failed");
						if (new_fd > 0)
						{
							std::cout << "New connection on listening socket " << res << std::endl;
							pfds[nfds].fd = new_fd;
							pfds[nfds].events = POLLIN;
							nfds++;
						}
					} while (new_fd != -1);
				}
				else
				{
					//recv and process request
					// conn_active = true;
					do
					{
						res = recv(pfds[i].fd, buf, sizeof(buf), 0);
						//...
					} while (1);
					
				}
			}
			else if (pfds[i].revents != 0)
			{
				servEnd = true;
				//close sockets;
				throw std::runtime_error("Error on poll: unexpected revents");
			}
		}
	}
	//close sockets;
	return;
}

void	ServerControler::sig_handler(int sig_num)
{
	(void)sig_num;
	g_serv_end = true;
	std::cout << "\nStoping server gracefully.\n";
}

void	ServerControler::createListeningSockets()
{
	int server_fd;
	//std::vector<int> ports;
	int	temp = 0;
	int port;
	int	size = _servBlocks.size();

	for (int i = 0; i < size; i++)
	{
		port = _servBlocks[i].getPort();
		if (port != temp)
		{
			server_fd = create_tcp_server_socket(port);
			//std::cout << "Port " << i << " :" << ports[i] << std::endl;
			_socketFds.push_back(server_fd);
			std::cout << "Server " << i << " listening on port: " << port << std::endl;
			temp = port;
		}
	}

}

void	ServerControler::processRequest(char *buf)
{
	(void)buf;
}

size_t		ServerControler::getServBlockNbr( void )
{
	return _servBlocks.size();
}


/*
** --------------------------------- ACCESSOR ---------------------------------
*/

void	ServerControler::setServer(const Server& instance)
{
	_servBlocks.push_back(instance);
}

const std::vector<Server>&	ServerControler::getServers( void ) const
{
	return (_servBlocks);
}

/* ************************************************************************** */
