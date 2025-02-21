#ifndef CONNECTION_HPP
# define CONNECTION_HPP

# include <string>
# include <ctime>

# define BUFF_SIZE 1500

class Connection
{
	public:

	Connection();
	Connection(int fd);
	~Connection();

	int		getFd() const;
	void	setFd(const int fd);
	time_t	getStartTime() const;
	void	setStartTime();
	bool	isTimeout();
	std::string	getRequest() const;
	void	setRequest(const std::string & s);
	void	appendRequest(const char * s);
	void	resetRequest();
	bool	isReqChuncked() const;
	void	setReqChuncked();
	size_t	getNextReqChunkSize() const;
	void	setNextReqChunkSize(const size_t size);
	std::string	getResponse() const;
	void	setResponse(const std::string & s);
	int		getReqHeadLen() const;

	void 	unchunkRequest();
	void	checkRequest();

	private:

	int	_fd;
	time_t _start;
	std::string _request;
	int	_req_body_len;
	int	_req_head_len;
	bool _req_chuncked;
	size_t _next_req_chunk;
	std::string _response;

};

struct Conn
{
	int	fd;
	bool active;
	time_t start;
	std::string request;
	bool req_complete;
	size_t next_req_chunk;
	std::string response;
	bool resp_complete;

};

#endif
