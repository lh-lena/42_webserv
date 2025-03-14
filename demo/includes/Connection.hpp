#ifndef CONNECTION_HPP
# define CONNECTION_HPP

# include <string>
# include <ctime>
# include <sys/socket.h>
# include <sys/types.h>
# include <netinet/in.h>
# include "RequestHandler.hpp"

# define BUFF_SIZE 1500

class Connection
{
	public:

	Connection();
	Connection(int fd);
	~Connection();

	int		getFd() const;
	void	setFd(const int fd);
	int		getPort() const;
	void	setPort(int p);
	struct sockaddr_in &getClientAddr();
	void	setClientAddr(struct sockaddr_in & client);
	time_t	getStartTime() const;
	void	setStartTime();
	bool	isTimeout();
	std::string	getRequest() const;
	void	setRequest(const std::string & s);
	void	appendRequest(const char * s);
	void	resetRequest();
	std::string	getResponse() const;
	void	setResponse(const std::string & s);
	void	setCGIHandler(RequestHandler & handler);
	RequestHandler & getCGIHandler();
	void	setCGIfds(int fds[2]);
	int[2]	getCGIfds();
	int		getCGIfdIn();
	int		getCGIfdOut();

	bool 	unchunkRequest();
	bool	checkRequest();

	private:

	int	_fd;
	int	_port;
	int	_cgi_fds[2];
	struct sockaddr_in _client_addr;
	time_t _start;
	std::string _request;
	size_t	_req_body_len;
	size_t	_req_head_len;
	bool _req_chuncked;
	std::string _response;
	RequestHandler _cgi_handler;

	size_t	getReqHeadLen();
	size_t	getReqBodyLen();
	bool	isReqChuncked();

};

//struct Conn
//{
//	int	fd;
//	bool active;
//	time_t start;
//	std::string request;
//	bool req_complete;
//	size_t next_req_chunk;
//	std::string response;
//	bool resp_complete;

//};

#endif
