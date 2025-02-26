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

bool	Connection::unchunkRequest() // return 1 if the last chunk received
{
	size_t	res;

	res = _request.find("\r\n0\r\n");
	if (res != std::string::npos)
		return 1;
	return 0;
}

size_t	Connection::getReqBodyLen()
{
	if (_req_body_len != 0)
		return _req_body_len;

	std::string s;
	size_t j = 0;

	size_t i = _request.find("Content-Length:");
	if (i == std::string::npos)
		return 0;
	if (_request[i + 15] == ' ')
		i++;
	i = i + 15;
	while (_request[i] != '\r')
		j++;
	if (j > 0)
	{
		s = _request.substr(i, j);
		_req_body_len = atol(s.c_str());
	}
	return _req_body_len;
}

bool	Connection::checkRequest() // return 1 if request fully received
{
	if (_request.empty())
		return 1;

	if (_req_head_len == 0)
	{
		if (getReqHeadLen() == 0)
			return 0;
	}
	if (isReqChuncked())
		return unchunkRequest();
	else if (getReqBodyLen() != 0)
	{
		if (_request.size() < _req_body_len + _req_head_len)
			return 0;
	}

	return 1;
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
		_request = s;
}

void	Connection::appendRequest(const char * s)
{
	if (s[0] != '\0')
		_request.append(s);
}

bool	Connection::isReqChuncked()
{
	if (_req_chuncked)
		return true;

	size_t res = _request.find("Transfer-Encoding:");
	if (res == std::string::npos)
		return false;
	if (_request[res + 18] == ' ')
		res++;
	res = res + 18;
	if (_request.compare(res, 7, "chunked"))
		_req_chuncked = true;
	return _req_chuncked;
}

void	Connection::resetRequest()
{
	_request = "";
	_req_body_len = 0;
	_req_head_len = 0;
	_req_chuncked = false;
	_next_req_chunk = 0;
}

size_t	Connection::getReqHeadLen()
{
	size_t	res;

	res = _request.find("\r\n\r\n");
	if (res != std::string::npos)
		_req_head_len = res + 4;

	return _req_head_len;
}

size_t	Connection::getChunkSize()
{
	return _next_req_chunk;
}

std::string	Connection::getResponse() const
{
	return _response;
}

void	Connection::setResponse(const std::string & s)
{
	_response = s;
}
