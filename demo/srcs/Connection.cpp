#include "Connection.hpp"

Connection::Connection()
{
	_start = time(NULL);
	_req_complete = false;
	// cgi_fds[0] = -1;
	// cgi_fds[1] = -1;
}

Connection::Connection(int fd): _fd(fd)
{
	_start = time(NULL);
	_req_complete = false;
	// cgi_fds[0] = -1;
	// cgi_fds[1] = -1;
}

Connection::~Connection()
{}

// std::string	Connection::unchunkRequest(char *buf)
// {
// 	if (!buf[0])
// 		return "";
// }

void	Connection::checkHeaders(char *buf) // what's happening in case of incomplete headers?
{
	size_t	res;
	std::string str;

	if (buf[BUFF_SIZE - 1] != '\0')
	{
		char c = buf[BUFF_SIZE - 1];
		buf[BUFF_SIZE - 1] = '\0';
		str = buf;
		str = str + c;
	}

	res = str.find("\r\n\r\n");
	if (res == std::string::npos)
		return;

	_req_head_len = res;
}

int		Connection::getFd() const
{
	return _fd;
}

void	Connection::setFd(const int fd)
{
	_fd = fd;
}

time_t	Connection::getStartTime() const
{
	return _start;
}

void	Connection::setStartTime()
{
	_start = time(NULL);
}

bool	Connection::isTimeout()
{
	time_t t = time(NULL);
	double dif = difftime(t, _start);
	if (dif > 30)
		return true;
	return false;
}

std::string	Connection::getRequest() const
{
	return _request;
}

void	Connection::setRequest(const std::string & s)
{
	if (!s.empty())
		_request = s;
}

void	Connection::appendRequest(const char * s)
{
	if (s[0] != '\0')
		_request.append(s);
}

bool	Connection::isReqComplete() const
{
	return _req_complete;
}

void	Connection::setReqComplete()
{
	_req_complete = true;
}

void	Connection::resetRequest()
{
	_request = "";
	_req_body_len = 0;
	_req_head_len = 0;
	_req_complete = false;
	_next_req_chunk = 0;
}

size_t	Connection::getNextReqChunkSize() const
{
	return _next_req_chunk;
}

void	Connection::setNextReqChunkSize(const size_t size)
{
	_next_req_chunk = size;
}

std::string	Connection::getResponse() const
{
	return _response;
}

void	Connection::setResponse(const std::string & s)
{
	_response = s;
}
