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
#include "Server.hpp"

#define READ_BUFFER_SIZE 1024
#define MAX_EVENTS  64
#define PORT 8080
#define LISTEN_BACKLOG 5
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define RESET "\033[0m"

std::vector<std::string> split(std::string &s, std::string delimeter)
{
    size_t pos = 0;
    std::vector<std::string> parts;
	if ((pos = s.find(delimeter)) == std::string::npos)
		parts.push_back(s);
    while ((pos = s.find(delimeter)) != std::string::npos)
    {
        std::string tocken = s.substr(0, pos);
        if (tocken.size() > 0)
        {
			parts.push_back(tocken);
            s.erase(0, pos + delimeter.length());
		}
    }
    return parts;
}

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

int create_tcp_server_socket()
{
	int server_fd;
	int reuse = 1; // Allow socket address reuse
	struct sockaddr_in socket_addr; // represents IPv4 socket addresses

	/* Step1: create a TCP socket */
	server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (server_fd == -1)
	{
		std::cerr << "Error: creation socket failed: " << strerror(errno) << std::endl;
		return -2;
	}

	/*This helps in manipulating options for the socket referred by the file descriptor sockfd. Optional*/
	int res = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char *) &reuse, sizeof(reuse)); 

	if (res == -1)
	{
		std::cerr << "Error: set socket options failed" << std::endl;
		close(server_fd);
		return -3;
	}

	/* Initialize the socket address structure */
	// bind the socket to a IP / port
	memset(&socket_addr, 0, sizeof(socket_addr));
	socket_addr.sin_family = AF_INET;
	socket_addr.sin_port = htons(PORT);
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
	return (server_fd);
}

void remove_fd(int fd)
{
	struct epoll_event epoll_event; 

	epoll_ctl(fd, EPOLL_CTL_DEL, fd, &epoll_event);
}

int forward_message(int fd, std::vector<std::string> parts)
{
	int ss = 0;
	for (int i = 0; i < parts.size(); i++)
	{
		ss = sizeof(parts[i]);
		if (ss)
			send(fd, parts[i].c_str(), ss + 1, 0);
	}
	return 0;
}

int recv_and_forward_message(int fd)
{
	std::string rem = "";

	ssize_t recv_size;
	while (1)
	{
        char buf[READ_BUFFER_SIZE];
        int recv_size = recv(fd, buf, READ_BUFFER_SIZE - 1, 0);

        if (recv_size > 0) {
            /* Read recv_size number of bytes from buf */
            std::string msg(buf, buf + recv_size);
            msg = rem + msg;

            /* Parse and split incoming bytes into individual messages */
            std::vector<std::string> parts = split(msg, "<EOM>");
            rem = msg;
			int ss = parts.size();
			for(int i = 0; i < ss; i++)
			{
				std::cout << "size: " << ss << " msg: " << parts[i] << std::endl;
			}
            forward_message(fd, parts);
        }
        else {
            /* Stopped sending new data */
            break;
        }
    }
	// recv_size = recv(fd, read_buffer, READ_BUFFER_SIZE -1, 0);
	// if (recv_size < 0)
	// {
	// 	std::cout << "Error: reading data: " << strerror(errno) << std::endl;
	// 	return -1;
	// }
	// else if (recv_size == 0)
	// {
	// 	std::cout << "Client disconnected" << std::endl;
    //     return 0;
	// }
	// read_buffer[recv_size] = '\0';
	// int bytes_read = read(fd, read_buffer, READ_BUFFER_SIZE -1);
	// printf("Bytes read: %d\n", bytes_read);
	// printf("Received message: %s\n", read_buffer);
	return 1;
}

int accept_new_connection_request(int server_fd, int epoll_fd, struct epoll_event* ev)
{
	struct sockaddr_in new_addr;
    int addrlen = sizeof(struct sockaddr_in);

    while (1)
	{
        /* Accept new connections */
        int conn_sock = accept(server_fd, (struct sockaddr*)&new_addr, 
                          (socklen_t*)&addrlen);
        
        if (conn_sock == -1) {
            /* We have processed all incoming connections. */
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
			{
                break;
            }
            else
			{
				std::cout << "Error accepting client connection: " << strerror(errno) << std::endl;
                break;
            }
        }

        /* Make the new connection non blocking */
        fcntl(conn_sock, F_SETFL, O_NONBLOCK);

        /* Monitor new connection for read events in edge triggered mode */
        ev->events = EPOLLIN | EPOLLET;
        ev->data.fd = conn_sock;

        /* Allow epoll to monitor new connection */
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, conn_sock, ev) == -1) {
			std::cout << "Error manipulating epoll instance: " << strerror(errno) << std::endl;
            break;
        }
    }
	return 0;
}

int	handle_event_on_file(struct epoll_event* epoll_event, int server_fd, int epoll_fd, struct epoll_event* ev)
{
	uint32_t events = epoll_event->events;
	int fd = epoll_event->data.fd;
	
	if (fd == server_fd)
	{
		/* New connection request received */
		accept_new_connection_request(fd, epoll_fd, ev);
	}
	if ((events & EPOLLERR) || 
		(events & EPOLLHUP) || 
		(!(events & EPOLLIN))) {
            /* Client connection closed */
			remove_fd(fd); // do we have to remove connection?
            close(fd);
	}
	else if (EPOLLIN & events)
	{
		/* Received data on an existing client socket */
		recv_and_forward_message(fd);
	}
	return 1;	
}

int create_epoll(int server_fd)
{
	/* create an epoll instance */
	int epoll_fd = epoll_create(1);
	if (epoll_fd == -1)
	{
		std::cout << "Error creating epoll instance: " << strerror(errno) << std::endl;
		return -1;
	}

	/*  Create an event buffer, which will be filled with events on opened files. */
	struct epoll_event ev, epoll_event_buffer[MAX_EVENTS];
	ev.events = EPOLLIN | EPOLLET; // Only poll for input events (reading) using edge triggered mode
	ev.data.fd = server_fd;

	/** Add this file descriptor to the list of descriptors to handle events on. */
	int ep_ctl = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev);
	if (ep_ctl == -1)
	{
		std::cout << "Error manipulating epoll instance: " << strerror(errno) << std::endl;
		close(server_fd);
		close(epoll_fd);
		return -1;
	}

	while(true)
	{
		// Call epoll_wait with a timeout of 1000 milliseconds to obtain number of file desciptors.
		int fds_ready = epoll_wait(epoll_fd, epoll_event_buffer, MAX_EVENTS, 1000); // -1 - infinite
		if(-1 == fds_ready)
		{
			std::cout << "Error: epoll_wait(): " << strerror(errno) << std::endl;
			break; // errors: EBADF or EINTR or EFAULT or EINVAL
		}
		/** Handle any file descriptors with events */
		for(int i = 0; i < fds_ready; i++)
		{
			struct epoll_event& epoll_event = epoll_event_buffer[i];
			// Handle the event now, by reading in data and printing it.
			if (handle_event_on_file(&epoll_event, server_fd, epoll_fd, &ev) == -1)
				return (-1);
		}
	}
	close(server_fd);
	close(epoll_fd);
	return 0;
}

int main()
{
	
	/* create listening socket */
    int server_fd = create_tcp_server_socket();
	if (!server_fd)
		return (server_fd);
	
	if (!set_to_nonblocking(server_fd))
		return -8;
	
	if (create_epoll(server_fd) < 0)
		return -1;

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

/*
https://copyconstruct.medium.com/the-method-to-epolls-madness-d9d2d6378642

https://mecha-mind.medium.com/a-non-threaded-chat-server-in-c-53dadab8e8f3
*/
