#include <string>
#include <vector>
#include <map>

enum Method {GET, POST, DELETE};

struct Request
{
	Method		method;
	std::string	host;
	std::string	fileName;
	std::string	reqBody;
	char**		envVars;
};
