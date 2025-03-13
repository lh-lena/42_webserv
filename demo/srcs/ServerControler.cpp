#include "../includes/ServerControler.hpp"
#include "../includes/Request.hpp"
#include "../includes/RequestHandler.hpp"
#include "../includes/CGI.hpp"
#include "../includes/utils.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstdlib>
#include <fcntl.h>
#include <cstring>
#include <csignal>

#define _XOPEN_SOURCE_EXTENDED 1

volatile bool g_serv_end = false;

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

ServerControler::ServerControler()
{
	struct rlimit lim;
	if (getrlimit(RLIMIT_NOFILE, &lim) == -1)
		throw std::runtime_error("Error: getrlimit() failed");
	if (lim.rlim_max < 100000)
		lim.rlim_cur = lim.rlim_max;
	else
		lim.rlim_cur = 100000;
	if (setrlimit(RLIMIT_NOFILE, &lim) == -1)
		throw std::runtime_error("Error: setrlimit() failed");
	if (getrlimit(RLIMIT_NOFILE, &lim) == -1)
		throw std::runtime_error("Error: getrlimit() failed");
	_pfds_limit = lim.rlim_cur;

	memset(_pfds, 0, sizeof(_pfds));

	std::cout << "Limit for the number of opened file descriptors: soft = "
				<< _pfds_limit << " hard = " << lim.rlim_max << std::endl;
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

int		ServerControler::isSocketFd(int fd)
{
	int size = _socketFds.size();
	for (int i = 0; i < size; i++)
	{
		if (_socketFds[i] == fd)
			return i;
	}
	return -1;
}

void	ServerControler::polling()
{
	int	res, indx;
	int	new_fd;
	int timeout = 5 * 60000;
	Connection *connection = NULL;
	struct sockaddr_in client;
	unsigned int size = sizeof(client);
	
	std::cout << "Waiting on poll" << std::endl;
	while (!g_serv_end)
	{
		res = poll(_pfds, _nfds, timeout);
		if (res < 0)
		{
			closeFds();
			throw std::runtime_error(strerror(errno));
		}
		if (res == 0)
		{
			std::cout << "Timeout" << std::endl;
			g_serv_end = true;
			break;
		}
		for (int i = 0; i < _nfds; i++)
		{
			connection = getConnection(_pfds[i].fd);
			if (_pfds[i].revents == 0)
			{
				if (connection && connection->isTimeout())
				{
					removeConnection(_pfds[i].fd);
					std::cout << "Connection on fd " << _pfds[i].fd << " was closed because of timeout" << std::endl;
				}
			}
			else if ((_pfds[i].revents & POLLHUP) || (_pfds[i].revents & POLLERR))
			{
				if (connection)
				{
					removeConnection(_pfds[i].fd);
					std::cout << "Connection on fd " << _pfds[i].fd << " was closed because of error or client disconnect" << std::endl;
				}
				else
					removePfd(_pfds[i].fd);
			}
			else if (_pfds[i].revents & POLLIN)
			{
				indx = isSocketFd(_pfds[i].fd);
				if (indx != -1)
				{
					memset(&client, 0, size);
					new_fd = accept(_pfds[i].fd, (sockaddr *)&client, &size);
					if (new_fd < 0 && errno != EWOULDBLOCK)
					throw std::runtime_error("Error: accept() failed");
					if (new_fd > 0)
					{
						addConnection(new_fd, _ports[indx], client);
						std::cout << "New connection on listening socket " << indx << ", new_fd = " << new_fd << std::endl;
						std::cout << "Client: " << utils::getClientIP(client) << ":" << utils::getClientPort(client) << std::endl;
					}
					continue;
				}
				else
					handleInEvent(_pfds[i].fd);
				if (_pfds[i].revents & POLLOUT) //&& connection
					handleOutEvent(_pfds[i].fd);
			}
		}
	}
	closeFds();
}

void	ServerControler::startServing()
{
	// signal(SIGINT, ServerControler::sig_handler);

	try
	{
		createListeningSockets();
	}
	catch(const std::exception& e)
	{
		closeFds();
		std::cerr << e.what() << '\n';
		throw std::runtime_error("Error: socket setup failed");
	}

	memset(_pfds, 0, sizeof(_pfds));
	size_t size = _socketFds.size();
	for (size_t i = 0; i < size; i++)
	{
		_pfds[i].fd = _socketFds[i];
		_pfds[i].events = POLLIN;
		_nfds++;
	}

	polling();
}

void	ServerControler::handleInEvent(int fd)
{
	//std::cout << "Handle POLLIN event on fd " << fd << std::endl;

	Connection *conn = getConnection(fd);
	if (conn == NULL)
	{
		std::cout << "handleInEvent: no such connection in conns" << std::endl;
		return;
	}

	char buf[BUFF_SIZE];
	memset(buf, 0, sizeof(buf));
	int res = recv(fd, buf, BUFF_SIZE - 1, 0);
	if (res < 0)
	{
		// std::cout << RED << "[ERROR] : "  << utils::getFormattedDateTime() << " recv() failed" << RESET << std::endl; //rm
		removeConnection(fd);
		return;
	}
	if (res > 0) // what if the last read res is exactly BUFF_SIZE - 1?
	{
		//std::cout << "Data received on connection " << fd << ": " << buf << std::endl;
		conn->appendRequest(buf);
		if (res < BUFF_SIZE - 1 || recv(fd, buf, 1, MSG_PEEK) < 1)
		{
			std::string request = conn->getRequest();
			std::cout << "Request on connection " << conn->getFd() << ":\n" << request << "size = " << request.size() << std::endl;
			if (conn->checkRequest())
			{
				processRequest(*conn);
				//std::string response = processRequest(request, conn->getPort());
				//conn->setResponse(response);
				conn->resetRequest();
			}
			else
				conn->setStartTime();
		}
	}
	else if (res == 0)
	{
		if (conn->isTimeout())
		{
			std::cout << "Connection on fd " << fd << " closed because of timeout" << std::endl;
			removeConnection(fd);
		}
	}
}

void	ServerControler::handleOutEvent(int fd)
{
	Connection *conn = getConnection(fd);
	if (conn == NULL)
	{
		std::cout << "handleOutEvent: no such connection in conns" << std::endl;
		return;
	}
	std::string str = conn->getResponse();
	if (str.empty())
		return;
	int res = send(fd, str.c_str(), str.size(), 0);
	if (res < 0)
	{
		removeConnection(fd);
		std::cerr << "Error: send() failed on connection fd " << fd << ". Connection closed." << std::endl;
		return;
	}

	conn->setResponse("");
	conn->setStartTime();

	std::cout <<"[INFO] : "  << utils::getFormattedDateTime() <<  " Transmitted Data Size "<< res <<" Bytes."  << std::endl;
	std::cout <<"[INFO] : "  << utils::getFormattedDateTime() <<  " File Transfer Complete." << std::endl;

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
			_ports.push_back(port);
			std::cout << "[INFO] : " << utils::getFormattedDateTime() << " Server " << i << " listening on port: " << port << std::endl;
			temp = port;
		}
	}

	std::cout << "Number of listening sokets created: " << _socketFds.size() << std::endl;
}

Server & ServerControler::chooseServBlock(const std::string & host, int port)
{
	int	res = 0;
	int size = _servBlocks.size();
	int i = 0;

	while (i < size && _servBlocks[i].getPort() != port)
		i++;

	res = i;

	while (i < size)
	{
		if (_servBlocks[i].getPort() != port && _servBlocks[i].getHost() == host)
			return _servBlocks[i];
		i++;
	}

	return _servBlocks[res];
}

void	ServerControler::processRequest(Connection & conn)
{
	Request		request;
	Response	response;
	Server		serv;

	// client_info : client IP adress and port
	struct sockaddr_in client_info = conn.getClientAddr();
	std::cerr << MAGENTA << "Processing request for client: " << utils::getClientIP(client_info) << ":"
				<< utils::getClientPort(client_info) << std::endl;

	// std::cerr << MAGENTA << data << RESET << std::endl; //rm

	if (!request.parse(conn.getRequest()))
	{
		if (request.getHeader("Server-Protocol") != "HTTP/1.1")
			response.setErrorResponse(505, std::string());
		else
			response.setErrorResponse(BAD_REQUEST, std::string());
		conn.setResponse(response.getResponse());
	}

	serv = chooseServBlock(request.getHeader("Host"), conn.getPort());
	RequestHandler reqHandler(serv, request, response);

	reqHandler.processRequest();

	// std::cerr << GREEN << response.getResponse() << RESET << std::endl;

	// std::ostringstream ss;
	// ne prazuie // did'ko
	std::string ss;
	// std::cout << MAGENTA << "[TRACE] "
	// << utils::getFormattedDateTime()
	// << "\"" << request.start_line << "\" "
	// << response.getStatusCode() << " "
	// << response.getHeader("Content-Length")  << RESET << std::endl;

	ss = "[TRACE] " + utils::getFormattedDateTime() + " \"" + request.start_line + "\"\n" +\
	 utils::itos(response.getStatusCode()) + " " + response.getHeader("Content-Length");

	std::cout << MAGENTA << ss << RESET << std::endl;
	conn.setResponse(response.getResponse());
}

std::string	ServerControler::processRequest(std::string & data, int port)
{
	Request		request;
	Response	response;
	Server		serv;
	// std::cerr << MAGENTA << data << RESET << std::endl; //rm
	if (!request.parse(data))
	{
		if (request.getHeader("Server-Protocol") != "HTTP/1.1")
			response.setErrorResponse(505, std::string());
		else
			response.setErrorResponse(BAD_REQUEST, std::string());
		return response.getResponse();
	}

	serv = chooseServBlock(request.getHeader("Host"), port);
	RequestHandler reqHandler(serv, request, response);

	reqHandler.processRequest();

	// std::cerr << GREEN << response.getResponse() << RESET << std::endl;

	// std::ostringstream ss;
	// ne prazuie // did'ko
	std::string ss;
	// std::cout << MAGENTA << "[TRACE] "
	// << utils::getFormattedDateTime()
	// << "\"" << request.start_line << "\" "
	// << response.getStatusCode() << " "
	// << response.getHeader("Content-Length")  << RESET << std::endl;

	ss = "[TRACE] " + utils::getFormattedDateTime() + " \"" + request.start_line + "\"\n" +\
	 utils::itos(response.getStatusCode()) + " " + response.getHeader("Content-Length");

	std::cout << MAGENTA << ss << RESET << std::endl;
	return response.getResponse();
}

/*
** --------------------------------- Accessors ----------------------------------
*/

size_t		ServerControler::getServBlockNbr( void )
{
	return _servBlocks.size();
}

void	ServerControler::setServer(const Server& instance)
{
	_servBlocks.push_back(instance);
}

const std::vector<Server>&	ServerControler::getServers( void ) const
{
	return (_servBlocks);
}

Connection	*	ServerControler::getConnection(int fd)
{
	int size = _conns.size();
	for (int i = 0; i < size; i++)
	{
		if (_conns[i].getFd() == fd)
			return &_conns[i];
	}
	return NULL;
}

void	ServerControler::addConnection(int fd, int port, struct sockaddr_in & client)
{
	if (_conns.size() == _work_conn_num)
	{
		std::cerr << "New connection not accepted. Maximum number of working connections (" << _work_conn_num <<") is reached" << std::endl;
		return;
	}

		Connection c(fd);
		c.setPort(port);
		c.setClientAddr(client);
		_conns.push_back(c);
		addPfd(fd);
}

void	ServerControler::removeConnection(int fd)
{
	int	size = _conns.size();
	if (!size)
		return ;
	for (int i = 0; i < size; i++)
	{
		if (_conns[i].getFd() == fd)
			_conns.erase(_conns.begin() + i);
	}
	removePfd(fd);
	close(fd);
}

void	ServerControler::addPfd(int fd)
{
	unsigned int i = _nfds;
	if (i == _pfds_limit)
	{
		std::cout << "The limit for opened file descriptors (" << _pfds_limit << ") is reached" << std::endl;
		return;
	}

	_pfds[i].fd = fd;
	_pfds[i].events = POLLIN | POLLOUT;
	_nfds++;

}

void	ServerControler::removePfd(int fd)
{
	int i = 0;

	while (_pfds[i].fd != fd && i < _nfds)
		i++;
	if (i == _nfds)
		return;

	if (close(_pfds[i].fd) == -1)
		throw std::runtime_error("Error: close() failed");

	for (int j = i; j < _nfds - 1; j++)
	{
		_pfds[j].fd = _pfds[j + 1].fd;
		_pfds[j].events = _pfds[j + 1].events;
		_pfds[j].revents = _pfds[j + 1].revents;
	}
	_nfds--;
	std::cout << "fd " << fd << " deleted from poll. Size of pollFd array is " << _nfds << std::endl;
}

void	ServerControler::setPfdEvent(int fd, char e)
{
	int i = 0;
	while (_pfds[i].fd != fd && i < _nfds)
		i++;
	if (i == _nfds)
		return ;
	if (e == 'o')
		_pfds[i].events = POLLOUT;
	else
		_pfds[i].events = POLLIN;
}

void	ServerControler::closeFds()
{
	for (int i = 0; i < _nfds; i++)
	{
		if (close(_pfds[i].fd) == -1)
			continue;
	}
}

void	ServerControler::setWorkConnNum(size_t n)
{
	_work_conn_num = n;
}


int	ServerControler::getNfds(void)
{
	return _nfds;
}

int	ServerControler::incrementNfds(void)
{
	if (_nfds < static_cast<int>(_pfds_limit))
		_nfds++;

	return _nfds;
}

int	ServerControler::decrementNfds(void)
{
	if (_nfds > 0)
		_nfds--;
	return _nfds;
}
