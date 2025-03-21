#include "Connection.hpp"
#include "utils.hpp"
#include "CGI.hpp"

Connection::Connection()
{
	_start = time(NULL);
	_req_body_len = 0;
	_req_head_len = 0;
	_req_chuncked = false;
	_cgi_handler = NULL;
	//memset(_cgi_fds, 0, sizeof(_cgi_fds));
}

Connection::Connection(int fd): _fd(fd)
{
	_start = time(NULL);
	_req_body_len = 0;
	_req_head_len = 0;
	_req_chuncked = false;
	_cgi_handler = NULL;
	//memset(_cgi_fds, 0, sizeof(_cgi_fds));
}

Connection::~Connection()
{
	if (_cgi_handler != NULL)
		delete _cgi_handler;
}

bool	Connection::unchunkRequest() // return 1 if the last chunk received
{
	size_t	res;
	std::string s;

	res = _request.find("\r\n0\r\n");
	if (res != std::string::npos)
	{
		_request.erase(res, 3);
		// replace "Transfer-Encoding: chunked" with "Content-Length: <_req_body_len>"
		return 1;
	}
	//getReqHeadLen();
	size_t i = _req_head_len + _req_body_len;
	//while (i < _request.size() && _request[i] < 48)
	//	i++;
	if (i >= _request.size())
		return 0;

	res = _request.find("\r\n", i);
	if (res != std::string::npos)
		return 0;
	s = _request.substr(i, res - i);
	res = strtol(s.c_str(), NULL, 16);
	if (res > 0)
	{
		_req_body_len = _req_body_len + res;
		_request.erase(i, s.size() + 2); //remove chunk-size-line from request
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
		std::cout << "Request headers length = " << _req_head_len << std::endl;
	}
	if (isReqChuncked())
		return unchunkRequest();
	else if (_request.find("POST") != std::string::npos)
	{
		getReqBodyLen();
		std::cout << "Request body length = " << _req_body_len << std::endl;
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
	_request.clear();
	_req_body_len = 0;
	_req_head_len = 0;
	_req_chuncked = false;

}

void	Connection::resetConnection()
{
	resetRequest();
	_response.clear();
	delete _cgi_handler;
	_cgi_handler = NULL;
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
