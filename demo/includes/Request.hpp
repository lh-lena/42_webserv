#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <vector>
#include <map>

struct Request
{
	std::string	method;
	std::string	host;
	std::string	reqBody;
	std::string	reqURI; /** the path must be extracted from the request */
	std::string	protocol;
	long long	contentLength;
	char**		envVars;

	Request() 
	{
		method = std::string();
		host = std::string();
		reqBody = std::string();
		reqURI = std::string();
		protocol = std::string();
		contentLength = 0;
	};
};

std::ostream &			operator<<( std::ostream & o, Request const & i );
// {
// 	o	<< "method = " << i.method << std::endl
// 		<< "host = " << i.host << std::endl
// 		<< "reqURI = " << i.reqURI << std::endl
// 		<< "protocol = " << i.protocol << std::endl
// 		<< "contentLength = " << i.contentLength << std::endl;

// 	return o;
// }

#endif
