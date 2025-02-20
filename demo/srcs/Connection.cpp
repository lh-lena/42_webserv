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

void	Connection::unchunkRequest()
{
	return;
}

void	Connection::checkRequest() // what's happening in case of incomplete headers?
{
	if (_request.empty())
		return;

	size_t	res;

	res = _request.find("\r\n\r\n");
	if (res != std::string::npos)
		_req_head_len = res + 4;

	res = _request.find("Content-Length");
	if (res != std::string::npos)
	{
		//_req_body_len = getContLength(&_request[i + 15]);
	}

	res = _request.find("Transfer-Encoding: chunked");
	if (res != std::string::npos)
		unchunkRequest();
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
