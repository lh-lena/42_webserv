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

bool	Request::parse(const std::string& data, const struct sockaddr_in & clientInfo)
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

	if (isValid() && isBodyExpected())
	{
		std::ostringstream oss;
		oss << iss.rdbuf();
		parseBody(oss.str());
	}

	parseClientInfo(clientInfo);

	return isValid();
}

bool		Request::isBodyExpected()
{
	return !getHeader("Content-Length").empty() || !getHeader("Transfer-Encoding").empty();
}

void	Request::parseStartLine(const std::string& str)
{
	start_line = str;
	if (utils::ends_with(start_line, "\r"))
	{
		start_line.erase(start_line.size() - 1);
	}
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
	parseReqUri(reqURI);
	setHeader("Request-Method", method);
	setHeader("Server-Protocol", protocol);
	setHeader("Query-String", query);
	_is_valid = true;
}

void	Request::parseHeader(const std::string& header_lines)
{
	std::string line = header_lines;

	if (utils::ends_with(line, "\r"))
	{
		line.erase(line.size() - 1);
	}
	size_t pos = line.find(":");
	if (pos == std::string::npos)
	{
		return;
	}

	std::string key = line.substr(0, pos);
	if (utils::ends_with(key, " "))
	{
		_is_valid = false;
		return;
	}
	std::string value = line.substr(pos + 1);
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

void	Request::parseReqUri(std::string uri)
{
	std::string path_info = utils::extract_path_info(uri);

	setHeader("Path-Info", path_info);
	if (!path_info.empty())
	{
		setHeader("Request-URI", utils::substr_before_rdel(uri, path_info));
	}
	else
	{
		setHeader("Request-URI", uri);
	}
}

void	Request::parseClientInfo(const struct sockaddr_in & clientInfo)
{
	setHeader("Remote-Addr", utils::getClientIP(clientInfo));
	setHeader("Remote-Port", utils::itos(utils::getClientPort(clientInfo)));
	setHeader("Remote-Ident", getHeader("Authorization"));
	setHeader("Remote-User", getHeader("Authorization"));
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
