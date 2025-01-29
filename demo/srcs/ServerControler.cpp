#include "../includes/ServerControler.hpp"
#include "../includes/Request.hpp"
#include "../includes/utils.hpp"

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

#define _XOPEN_SOURCE_EXTENDED 1
#define MAX_CONN_NUM 200

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
	std::string str;
	std::string request;
	struct pollfd pfds[MAX_CONN_NUM]; //max number of connections
	int nfds = 0;
	int new_fd = -1;
	char buf[1500];
	//bool conn_active = false;

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

	timeout = 3 * 60000;
	servEnd = false;
	std::cout << "Waiting on poll" << std::endl;
	while (!servEnd)
	{
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
			servEnd = true;
			//close(_socketFds);
			break;
		}
		std::cout << "nfds = " << nfds << std::endl;
		for (int i = 0; i < nfds; i++)
		{
			if (pfds[i].revents == POLLIN)
			{
				std::cout << "POLLIN on poll_fd " << i << std::endl;
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
					//conn_active = true;
					request = "";
					str = "";
					do
					{
						memset(buf, 0, sizeof(buf));
						res = recv(pfds[i].fd, buf, sizeof(buf), 0);
						if (res < 0)
						{
							std::cout << "Error: recv() failed" << std::endl;
							break;
						}
						if (res > 0)
						{
							request.append(buf);
						}
					} while (res == 1500);
					if (!request.empty())
					{
						str = processRequest(request);

						if (!str.empty())
						{
							res = send(pfds[i].fd, str.c_str(), str.size(), 0);
							if (res < 0)
								throw std::runtime_error("Error: send() failed");
							std::cout <<"[LOG] : Transmitted Data Size "<< res <<" Bytes."  << std::endl;
							std::cout <<"[LOG] : File Transfer Complete." << std::endl;
						}
					}
					//conn_active = false;
				}
			}
			else if (pfds[i].revents == POLLHUP)
			{
				std::cout << "POLLHUP on poll_fd " << i << std::endl;
				close(pfds[i].fd);
				pfds[i].fd = -1;
			}
			else if (pfds[i].revents == POLLERR)
			{
				servEnd = true;
				//close sockets;
				std::cout << "Unexpected revents on poll_fd " << i << ": pfds[i].revents" << std::endl;
				break;
			}
			else
			{
				std::cout << "Poll_fd " << i << " revents = " << pfds[i].revents << std::endl;
				//if (i == nfds - 1)
				//	servEnd = true;
			}
			for (int n = 0; n < nfds; n++)
			{
				if (pfds[n].fd == -1)
				{
					for(int j = n; j < nfds - 1; j++)
					{
						pfds[j].fd = pfds[j+1].fd;
					}
					n--;
					nfds--;
				}
			}
		}
	}
	//close sockets;
	for (int i = 0; i < nfds; i++)
	{
		if(pfds[i].fd >= 0)
		close(pfds[i].fd);
	}
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

// static int parseRequest(std::string & data, Request & req)
// {
// 	//read data and fill request fields
// 	// check for valid request, return error number

// 	return 0;
// }

Server & ServerControler::chooseServBlock(std::string & host)
{
	for (size_t i = 0; i < _servBlocks.size(); i++)
	{
		if (host == _servBlocks[i].getHost())
			return _servBlocks[i];
	}
	return _servBlocks[0];
}

std::string	ServerControler::processRequest(std::string & data)
{
	// Request request;

	// int res = parseRequest(data, request);
	// if (res)
	// 	return ("Error: invalid request");

	// Server & server = chooseServBlock(request.host);

	//std::string response = "Hello\n";
	std::cout << data << std::endl;

	std::string str;
	Request req;
	Response resp;
	extractPath(data, req.method_r, req.reqURI);
	Server serv = getServers()[0];
	serv.handleRequestMethod(req, resp);
	serv.createResponse(resp, str);

	return str;
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
