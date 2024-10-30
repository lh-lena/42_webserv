#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string>
#include <cstring>
#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib>
#include <fcntl.h>
#include <sys/epoll.h>

#define READ_BUFFER_SIZE 1024
#define PORT 8080
#define LISTEN_BACKLOG 5
#define MAX_EVENTS 32
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define RESET "\033[0m"

int set_to_nonblocking(int server_fd)
{
	// Set server socket to non-blocking
	int flags = fcntl(server_fd, F_GETFL, 0);
	if (flags == -1)
	{
		std::cerr << "Error: " << strerror(errno) << std::endl;
		close(server_fd);
		return -6;
	}
	int res = fcntl(server_fd, F_SETFL, flags | O_NONBLOCK);
	if (flags == -1)
	{
		std::cerr << "Error: " << strerror(errno) << std::endl;
		close(server_fd);
		return -7;
	}
	return 1;
}

int create_listening_socket()
{
	int server_fd;
	int reuse = 1; // Allow socket address reuse
	struct sockaddr_in server_addr, client_addr; // represents IPv4 socket addresses
	socklen_t client_size = sizeof(client_addr); 
	char buffer[READ_BUFFER_SIZE] = { 0 };

	// create and bind the listening socket
	server_fd = socket(AF_INET, SOCK_STREAM, 0);

	if (server_fd == -1)
	{
		std::cerr << "Error creating socket: " << strerror(errno) << std::endl;
		return -2;
	}

	/*This helps in manipulating options for the socket referred by the file descriptor sockfd. Optional*/
	int res = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char *) &reuse, sizeof(reuse)); // | SO_REUSEPORT 

	if (res < 0)
	{
		std::cerr << "Socket creation failed" << std::endl;
		return -3;
	}

	// bind the socket to a IP / port
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY; // Bind to any available interface
	/*
	//ip address of www.msn.com (get by doing a ping www.msn.com at terminal)
	server.sin_addr.s_addr = inet_addr("131.253.13.140");
	if (server.sin_addr.s_addr != INADDR_NONE) // good
	*/

	server_addr.sin_port = htons(PORT);

	// associates the socket with a specific IP address and port number
	if (bind(server_fd, (sockaddr*)&server_addr, sizeof(sockaddr)) == -1)
	{
		std::cerr << "Error binding socket: " << strerror(errno) << std::endl;
		close(server_fd);
		return -4;
	}

	// Listen for connections. Max connections is 5
	if (listen(server_fd, LISTEN_BACKLOG) == -1)
	{
		std::cerr << "Error listening on socket: " << strerror(errno) << std::endl;
		close(server_fd);
		return -5;
	}

	std::cout << "Server listening on port " << PORT << std::endl;

	if (!set_to_nonblocking(server_fd))
		return -8;
	return (server_fd);

	// Accept connections
	// if (accept(server_fd, (sockaddr*)&client_addr, &client_size) == -1)
	// {
	// 	std::cerr << "Error accepting connection " << strerror(errno) << std::endl;
	// 	close(server_fd);
	// 	return -5;
	// }


	// Get client's IP address and port
	// Send data to the client
	// Close sockets
}

void remove_fd(int fd)
{
	struct epoll_event epoll_event; 

	epoll_ctl(fd, EPOLL_CTL_DEL, fd, &epoll_event);
}

int	handle_event_on_file(struct epoll_event* epoll_event)
{
	uint32_t events = epoll_event->events;
	int fd = epoll_event->data.fd;
	ssize_t recv_size;
	char read_buffer[READ_BUFFER_SIZE];
	if (EPOLLERR & events)
	{
		remove_fd(fd);
		close(fd);
	}
	else if (EPOLLIN & events)
	{
		recv_size = recv(fd, read_buffer, READ_BUFFER_SIZE -1, 0);
		if (recv_size < 0)
		{
			std::cout << "Error creating epoll instance: " << strerror(errno) << std::endl;
			return -1;
		}
		read_buffer[recv_size] = '\0';
	}

	int bytes_read = read(fd, read_buffer, READ_BUFFER_SIZE -1);
	printf("Bytes read: %d", bytes_read);
	printf("%s", read_buffer);
}

int main()
{
	//  create an epoll instance
	int epoll_fd = epoll_create(1);

	if (epoll_fd == -1)
	{
		std::cout << "Error creating epoll instance: " << strerror(errno) << std::endl;
		return -1;
	}
	// create listening socket
    int listen_sock_addr = create_listening_socket();
	if (!listen_sock_addr)
		return (listen_sock_addr);

	// Add this file descriptor to the list of descriptors to handle events on.
// First, create a struct describing the information of the open file 
// (file descriptor, whether to track input/output, etc.)
	struct epoll_event ev;
	ev.events = EPOLLIN; // Only poll for input events (reading).
	ev.data.fd = listen_sock_addr;
	int ep_ctl = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_sock_addr, &ev);
	if (ep_ctl == -1)
	{
		std::cout << "Error manipulating epoll instance: " << strerror(errno) << std::endl;
		return -1;
	}

	// if (connect(listen_sock_addr, (sockaddr*)&))

	// Create an event buffer, which will be filled with events on opened files.
	size_t EVENT_BUFFER_SIZE = 64;
	struct epoll_event epoll_event_buffer[EVENT_BUFFER_SIZE];

	while(true)
	{
		// Call epoll_wait with a timeout of 1000 milliseconds to obtain number of file desciptors.
		int fds_ready = epoll_wait(epoll_fd, epoll_event_buffer, EVENT_BUFFER_SIZE, 1000);
		// Function epoll_wait returned.
		if(-1 == fds_ready)
		{
			// An error occured.
			break;
		}
		// Handle any file descriptors with events.
		for(int i = 0; i < fds_ready; i++)
		{
			struct epoll_event& epoll_event = epoll_event_buffer[i];
			// Handle the event now, by reading in data and printing it.
			if (handle_event_on_file(&epoll_event) == -1)
				return (-1);
			
		}
	}

	return (0);
}

/*
epoll: https://daankolthof.com/2019/08/01/asynchronous-file-i-o-on-linux-the-epoll-api/

remove file descriptor:
// For the sake of compatibility with older versions of Linux, this struct must exist (pointer to struct cannot be nullptr).
struct epoll_event epoll_event; 

epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, &epoll_event);
 */

/*
std::string upload_dir = "/var/www/uploads/"; // Configured upload directory

// Extract file from the request (e.g., in POST request body)
std::string filename = extract_filename_from_multipart_body(request_body);

// Save the file
std::ofstream outfile(upload_dir + filename, std::ios::binary);
outfile.write(file_content.data(), file_content.size());
outfile.close();
*/