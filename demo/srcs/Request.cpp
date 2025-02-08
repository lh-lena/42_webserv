#include "../includes/Request.hpp"
#include "../includes/Location.hpp"
#include "../includes/RequestHandler.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Request::Request()
{}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Request::~Request() {}

/*
** --------------------------------- METHODS ----------------------------------
*/

bool	Request::parse(const std::string& data)
{
	std::istringstream	iss(data);
	std::string			line, name, value;

	if (std::getline(iss, line))
	{
		parseStartLine(line);
	}
	else
	{
		return false;
	}

	while (std::getline(iss, line) && !line.empty())
	{
		parseHeader(line);
	}

	std::ostringstream oss;
	oss << iss.rdbuf();
	std::cerr << "BODY: " << oss.str() << std::endl; // rm
	parseBody(oss.str());

	return true;
}

void	Request::parseStartLine(const std::string& str)
{
	std::istringstream iss(str);

	iss >> _method >> _reqURI >> _protocol;
	_reqURI = RequestHandler::canonicalizePath(_reqURI);
	utils::parse_query(_reqURI, _reqURI, _query);
}

void	Request::parseHeader(const std::string& header_lines)
{
	size_t pos = header_lines.find(":");
	if (pos == std::string::npos)
	{
		return;
	}

	std::string key = header_lines.substr(0, pos);
	std::string value = header_lines.substr(pos + 2);

	_headers[key] = value;
}

void	Request::parseBody(const std::string& body)
{
	_body = body;
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

std::string		Request::getMethod() const
{
	return _method;
}

std::string		Request::getURI() const
{
	return _reqURI;
}

std::string		Request::getQueryString() const
{
	return _query;
}

std::string		Request::getProtocol() const
{
	return _protocol;
}

std::string		Request::getHeader(const std::string& key) const
{
	if (_headers.find(key) == _headers.end())
	{
		return std::string();
	}

	std::map<std::string, std::string>::const_iterator	it = _headers.find(key);
	return it->second;
}

std::string		Request::getBody() const
{
	return _body;
}

std::ostream &			operator<<( std::ostream & o, Request const & i )
{
	o	<< "method = " << i.getMethod() << std::endl
		<< "reqURI = " << i.getURI() << std::endl
		<< "protocol = " << i.getProtocol() << std::endl
		<< "query = " << i.getQueryString() << std::endl
		<< "body = " << i.getBody();

	return o;
}

