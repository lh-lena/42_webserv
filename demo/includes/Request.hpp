#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <vector>
#include <map>

enum Method {GET, POST, DELETE, UNSUPPORTED};

struct Request
{
	Method		method;
	std::string	method_r; /** added requested method as a string, for further checking */
	std::string	host;
	std::string	fileName;
	std::string	reqBody;
	std::string	reqURI; /** the path must be extracted from the request */
	char**		envVars;
};

#endif
