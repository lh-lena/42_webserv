#include "Connection.hpp"
#include "utils.hpp"
#include "CGI.hpp"

Connection::Connection()
{
	_request = "";
	_response = "";
	_start = time(NULL);
	_req_body_len = 0;
	_req_head_len = 0;
	_req_chunked = false;
	_cgi_handler = NULL;
	_cgi_fail = false;

}

Connection::Connection(int fd): _fd(fd)
{
	_request = "";
	_response = "";
	_start = time(NULL);
	_req_body_len = 0;
	_req_head_len = 0;
	_req_chunked = false;
	_cgi_handler = NULL;
	_cgi_fail = false;

}

Connection::~Connection()
{
	if (_cgi_handler != NULL)
	{
		delete _cgi_handler;
	}
}

bool	Connection::unchunkRequest()
{
	size_t pos = 0;
	size_t pos_prev = _req_head_len;
	int i = 0;
	bool end = false;
	std::string	chunk_size;

	while (!end)
	{
		pos = _request.find("\r\n", pos_prev);
		if (pos == std::string::npos)
		{
			return 0;
		}
		_request.erase(pos, 2);
		if (i % 2 == 0)
		{
			chunk_size = _request.substr(pos_prev, pos - pos_prev);
			_request.erase(pos_prev, chunk_size.length());
			if (chunk_size == "0")
			{
				pos = _request.find("\r\n", pos_prev);
				if (pos == std::string::npos)
				{
					return 0;
				}
				_request.erase(pos, 2);
				pos_prev = pos;
				end = true;
				return 1;
			}
		}
		pos_prev = pos;
		i++;
	}
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
	j = i;
	while (j < _request.size() && _request[j] != '\r')
		j++;
	if (j > 0)
	{
		s = _request.substr(i, j);
		_req_body_len = utils::stoull(s);
	}
	return _req_body_len;
}

bool	Connection::checkRequest() // return 1 if request fully received
{
	if (_request.empty())
		return 1;

	if (_req_head_len == 0)
	{
		getReqHeadLen();
		// std::cout << "Request headers length = " << _req_head_len << std::endl;
	}
	if (isReqChunked())
	{
		int check = unchunkRequest();
		return check;
	}
	if (_request.find("POST") != std::string::npos)
	{
		getReqBodyLen();
		// std::cout << "Request body length = " << _req_body_len << std::endl;
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

int		Connection::getPort() const
{
	return _port;
}

void	Connection::setPort(int p)
{
	_port = p;
}

void	Connection::setClientAddr(struct sockaddr_in & client)
{
	_client_addr = client;
}

struct sockaddr_in &Connection::getClientAddr()
{
	return _client_addr;
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
	return utils::isTimeout(_start, 30);
}

std::string	Connection::getRequest() const
{
	return _request;
}

void	Connection::setRequest(const std::string & s)
{
		_request = s;
}

void	Connection::appendRequest(const char * s, int res)
{
	_request.append(s, res);
}

bool	Connection::isReqChunked()
{
	if (_req_chunked)
		return true;

	size_t res = _request.find("Transfer-Encoding:");
	if (res == std::string::npos)
	return false;

	if (_request[res + 18] == ' ')
		res++;
	res = res + 18;

	if (_request.compare(res, 7, "chunked") == 0)
		_req_chunked = true;

	return _req_chunked;
}

void	Connection::resetRequest()
{
	if (!_request.empty())
		_request.clear();
	_req_body_len = 0;
	_req_head_len = 0;
	_req_chunked = false;

}

void	Connection::resetConnection()
{
	resetRequest();
	_response.clear();
	if (_cgi_handler != NULL)
	{
		delete _cgi_handler;
		_cgi_handler = NULL;
	}
	setStartTime();
}

size_t	Connection::getReqHeadLen()
{
	size_t	res;

	res = _request.find("\r\n\r\n");
	if (res != std::string::npos)
		_req_head_len = res + 4;
	else
		_req_head_len = _request.size();

	return _req_head_len;
}

std::string	Connection::getResponse() const
{
	return _response;
}

void	Connection::setResponse(const std::string & s)
{
	_response = s;
}

void	Connection::setCGIHandler(RequestHandler * handler)
{
	_cgi_handler = handler;
}

RequestHandler * Connection::getCGIHandler()
{
	return _cgi_handler;
}

int		Connection::getCGIfdIn()
{
	if (_cgi_handler != NULL)
		return _cgi_handler->getCGI().getIfd();
	return -1;
}

int		Connection::getCGIfdOut()
{
	if (_cgi_handler != NULL)
		return _cgi_handler->getCGI().getOfd();
	return -1;
}

void	Connection::setCGIfail(bool b)
{
	_cgi_fail = b;
}

bool	Connection::getCGIfail()
{
	return _cgi_fail;
}
