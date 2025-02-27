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
	start_line = str;
	std::istringstream iss(str);
	std::string	method, reqURI, protocol, query;

	iss >> method >> reqURI >> protocol;
	if (method.empty() || reqURI.empty() || protocol.empty())
	{
		_is_valid = false;
		return;
	}
	reqURI = RequestHandler::canonicalizePath(reqURI);
	utils::parse_query(reqURI, reqURI, query);
	setHeader("Request-Method", method);
	setHeader("Request-URI", reqURI);
	setHeader("Server-Protocol", protocol);
	setHeader("Query-String", query);
	_is_valid = true;
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
	setHeader(key, value);
	_is_valid = true;
}

void	Request::parseBody(const std::string& body)
{
	_body = body;
}

std::vector<std::pair<std::string, std::string> >	Request::getHeaders() const
{
	return _header_fields;
}


/*
** --------------------------------- ACCESSOR ---------------------------------
*/

void	Request::setHeader(const std::string& key, const std::string& value)
{
	_header_fields.push_back(std::make_pair(key, value));
}

void					Request::setFullPath(const std::string& path)
{
	_full_path = path;
}

// void					Request::setUploadPath(const std::string& path)
// {
// 	_upload_path = path;
// }

const std::string		Request::getHeader(const std::string& key) const
{
	std::vector<std::pair<std::string, std::string> >::const_iterator it = _header_fields.begin();
	for (; it != _header_fields.end(); ++it)
	{
		if (it->first == key)
		{
			return it->second;
		}
	}
	return "";
}

void	Request::printHeaders() const
{
	std::vector<std::pair<std::string, std::string> >::const_iterator it = _header_fields.begin();
	for (; it != _header_fields.end(); ++it)
	{
		std::cout << it->first << ":" << it->second << std::endl;
	}
}

const std::string&		Request::getBody() const
{
	return _body;
}

const std::string&		Request::getFullPath() const
{
	return _full_path;
}

// const std::string&		Request::getUploadPath() const
// {
// 	return _upload_path;
// }

std::ostream &			operator<<( std::ostream & o, Request const & i )
{
	o	<< "method = " << i.getHeader("Request-Method") << std::endl
		<< "reqURI = " << i.getHeader("Request-URI") << std::endl
		<< "fullPath = " << i.getFullPath() << std::endl
		<< "protocol = " << i.getHeader("Server-Protocol") << std::endl
		<< "query = " << i.getHeader("Query-String") << std::endl
		<< "upload path " << i._upload_path << std::endl
		<< "body = " << i.getBody() << std::endl
		;
	return o;
}
