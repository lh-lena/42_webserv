#include "../includes/ServerControler.hpp"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netdb.h>
#include <unistd.h>
#include <cstdlib>
#include <fcntl.h>
#include <cstring>
#include <vector>
#include <stdexcept>

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

ServerControler::ServerControler()
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
	for (Server server : servers)
	{
		o << server << std::endl;
	}
	return o;
}

/*
** --------------------------------- STATIC -----------------------------------
*/
static std::vector<int>	getPorts(std::vector<Server> & servers)
{
	std::vector<int> ports;
	int	port;

	int	size = servers.size();

	if (!size)
		throw std::invalid_argument("Bad configuration file"); // check for parcing error

	ports.push_back(servers[0].getPort());

	for (int i = 1; i < size; i++)
	{
		port = servers[i].getPort();
		if (port != ports[i - 1])
			ports.push_back(port);
	}
	return ports;
}

/*
** --------------------------------- METHODS ----------------------------------
*/
void	ServerControler::createListeningSockets()
{
	std::vector<int> ports = getPorts(this->_servBlocks);
	int size = ports.size();
	int server_fd;
	int reuse = 1; // Allow socket address reuse
	struct sockaddr_in socket_addr; // represents IPv4 socket addresses

	for (int i = 0; i < size; i++)
	{
		/* Step1: create a TCP socket */
		server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (server_fd == -1)
		{
			std::cerr << "Error: creation socket failed: " << strerror(errno) << std::endl;
			return;
		}

		/*This helps in manipulating options for the socket referred by the file descriptor sockfd. Optional*/
		int res = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char *) &reuse, sizeof(reuse));

		if (res == -1)
		{
			std::cerr << "Error: set socket options failed" << std::endl;
			close(server_fd);
			return;
		}

		/* Initialize the socket address structure */
		// bind the socket to a IP / port
		memset(&socket_addr, 0, sizeof(socket_addr));
		socket_addr.sin_family = AF_INET;
		socket_addr.sin_port = htons(ports[i]);
		socket_addr.sin_addr.s_addr = INADDR_ANY; // Bind to any available interface
		/*
		//ip address of www.msn.com (get by doing a ping www.msn.com at terminal)
		server.sin_addr.s_addr = inet_addr("131.253.13.140");//INADDR_ANY
		if (server.sin_addr.s_addr != INADDR_NONE) // good
		*/

		// associates the socket with a specific IP address and port number
		if (bind(server_fd, (sockaddr*)&socket_addr, sizeof(sockaddr)) == -1)
		{
			std::cerr << "Error: binding socket failed: " << strerror(errno) << std::endl;
			close(server_fd);
			return;
		}

		// Listen for connections. Max connections is 5 LISTEN_BACKLOG
		if (listen(server_fd, SOMAXCONN) == -1)
		{
			std::cerr << "Error listening on socket: " << strerror(errno) << std::endl;
			close(server_fd);
			return;
		}

		_socketFds.push_back(server_fd);
		std::cout << "Server " << i << " listening on port: " << ports[i] << std::endl;
	}

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
