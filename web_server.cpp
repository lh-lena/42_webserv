#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string>
#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib>
#include <fcntl.h>

#define BUFFER_SIZE 1024
#define PORT 8080
#define LISTEN_BACKLOG 5
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define RESET "\033[0m"

int main()
{
	// create a socket
	int server_fd;
	int reuse = 1;
	struct sockaddr_in server_addr, client_addr; // represents IPv4 socket addresses
	socklen_t client_size = sizeof(client_addr); 
	char buffer[BUFFER_SIZE] = { 0 };

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1)
	{
		std::cerr << "Error creating socket" << std::endl;
		return -1;
	}

	/*This helps in manipulating options for the socket referred by the file descriptor sockfd. Optional*/
	int res = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (char *) &reuse, sizeof(reuse));
	if (res < 0)
	{
		std::cerr << "Socket creation failed" << std::endl;
		return -2;
	}

	// bind the socket to a IP / port
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY; // Bind to any available interface
	/*
	//ip address of www.msn.com (get by doing a ping www.msn.com at terminal)
	server.sin_addr.s_addr = inet_addr("131.253.13.140");
	*/
	server_addr.sin_port = htons(PORT);

	// associates the socket with a specific IP address and port number
	if (bind(server_fd, (sockaddr*)&server_addr, sizeof(sockaddr)) == -1)
	{
		std::cerr << "Can't bind to IP/port" << std::endl;
		close(server_fd);
		return -3;
	}

	 // Listen for connections. Max connections is 5
	if (listen(server_fd, LISTEN_BACKLOG) == -1)
	{
		std::cerr << "Can't listen!" << std::endl;
		close(server_fd);
		return -4;
	}
	std::cout << "Server listening on port " << PORT << std::endl;

	// Set server socket to non-blocking
	int flags = fcntl(server_fd, F_GETFL, 0);
	fcntl(server_fd, F_SETFL, flags | O_NONBLOCK);


	// Accept connections
	if (accept(server_fd, (sockaddr*)&client_addr, &client_size) == -1)
	{
		std::cerr << "Error accepting connection" << std::endl;
		close(server_fd);
		return -5;
	}

	// Get client's IP address and port
	// Send data to the client
	// Close sockets
	return (0);
}

/*
std::string upload_dir = "/var/www/uploads/"; // Configured upload directory

// Extract file from the request (e.g., in POST request body)
std::string filename = extract_filename_from_multipart_body(request_body);

// Save the file
std::ofstream outfile(upload_dir + filename, std::ios::binary);
outfile.write(file_content.data(), file_content.size());
outfile.close();
*/