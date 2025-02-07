#include "../includes/Request.hpp"
#include "../includes/Location.hpp"
#include "../includes/RequestHandler.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Request::Request(): contentLength(0)
{}

// Request::Request():
// 	method(std::string()),
// 	host(std::string()),
// 	reqBody(std::string()),
// 	reqURI(std::string()),
// 	protocol(std::string()),
// 	query(std::string()),
// 	charset(std::string()),
// 	contentType(std::string()),
// 	fileUpload(std::string()),
// 	dirUpload(std::string()),
// 	cgiInterpreter(std::string()),
// 	contentLength(0)
// {}

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

	/* change to read headers */
	while (std::getline(iss, line))
	{
		std::istringstream tmp(line);
		// std::cout << "LINE " << line << std::endl;
		tmp >> name >> value;

		if (name.compare("Host:") == 0)
		{
			host = value;
		}
		else if (name.compare("Content-Length:") == 0)
		{
			contentLength = utils::strToUlong(value);
		}
		else if (name.compare("Content-Type:") == 0)
		{
			contentType = value;
			// Content-Type: multipart/form-data;boundary="delimiter12345"
		}
	}

	/* read body */

	return true;
}

void	Request::parseStartLine(const std::string& str)
{
	std::istringstream iss(str);

	iss >> method >> reqURI >> protocol;
	reqURI = RequestHandler::canonicalizePath(reqURI);
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/

std::ostream &			operator<<( std::ostream & o, Request const & i )
{
	o	<< "method = " << i.method << std::endl
		<< "host = " << i.host << std::endl
		<< "reqURI = " << i.reqURI << std::endl
		<< "protocol = " << i.protocol << std::endl
		<< "contentLength = " << i.contentLength << std::endl;

	return o;
}

