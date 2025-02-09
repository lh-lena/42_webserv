#include "../includes/Request.hpp"
#include "../includes/Location.hpp"
#include "../includes/RequestHandler.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Request::Request() : _is_valid(false)
{}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Request::~Request() {}

/*
** --------------------------------- METHODS ----------------------------------
*/

bool	Request::isValid( void )
{
	return _is_valid;
}

bool	Request::parse(const std::string& data)
{
	std::istringstream	iss(data);
	std::string			line;

	if (std::getline(iss, line))
	{
		parseStartLine(line);
	}
	else
	{
		return isValid();
	}

	while (std::getline(iss, line) && line.compare("\r") != 0)
	{
		parseHeader(line);
	}

	// to determine if a message body is expected
	if (isValid() && isBodyExpected())
	{
		std::ostringstream oss;
		oss << iss.rdbuf();
		parseBody(oss.str());
	}

	return isValid();
}

bool		Request::isBodyExpected()
{
	return !getHeader("Content-Length").empty() || !getHeader("Transfer-Encoding").empty();
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
	if (utils::ends_with(key, " "))
	{
		_is_valid = false;
		return;
	}
	std::string value = header_lines.substr(pos + 1);
	if (std::strncmp(value.c_str(), " ", 1) == 0)
	{
		value.erase(0, 1);
	}
	_header_fields[key] = value;
	_is_valid = true;
}

/*
If a Transfer-Encoding header field
is present in a request and the chunked transfer coding is not
the final encoding, the message body length cannot be determined
reliably; the server MUST respond with the 400 (Bad Request)
status code and then close the connection.
*/

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
	if (_header_fields.find(key) == _header_fields.end())
	{
		return std::string();
	}

	std::map<std::string, std::string>::const_iterator	it = _header_fields.find(key);
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

