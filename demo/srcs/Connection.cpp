#include "Connection.hpp"

Connection::Connection()
{
	_start = time(NULL);
	_req_chuncked = false;
}

Connection::Connection(int fd): _fd(fd)
{
	_start = time(NULL);
	_req_chuncked = false;
}

Connection::~Connection()
{}

void	Connection::unchunkRequest()
{
	return;
}

static bool isChunked(std::string & str)
{
	size_t res = str.find("Transfer-Encoding:");
	if (res == std::string::npos)
		return false;
	if (str[res + 18] == ' ')
		res++;
	res = res + 18;
	if (str.compare(res, 7, "chunked"))
		return true;
	return false;
}

void	Connection::checkRequest() // what's happening in case of incomplete headers?
{
	if (_request.empty())
		return;

	size_t	res;

	res = _request.find("\r\n\r\n");
	if (res != std::string::npos)
		_req_head_len = res + 4;
	else
		_req_head_len = _request.size();

	if (isChunked(_request))
		_req_chuncked = true;
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

bool	Connection::isReqChuncked() const
{
	return _req_chuncked;
}

void	Connection::setReqChuncked()
{
	_req_chuncked = true;
}

void	Connection::resetRequest()
{
	_request = "";
	_req_body_len = 0;
	_req_head_len = 0;
	_req_chuncked = false;
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

int		Connection::getReqHeadLen() const
{
	return _req_head_len;
}
