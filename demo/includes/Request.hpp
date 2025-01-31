#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <string>
#include <vector>
#include <map>

struct Request
{
	std::string	method_r;
	std::string	host;
	std::string	reqBody;
	std::string	reqURI; /** the path must be extracted from the request */
	char**		envVars;
};

#endif
