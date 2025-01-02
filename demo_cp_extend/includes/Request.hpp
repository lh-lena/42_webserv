#include <string>
#include <map>

enum Method {GET, POST, DELETE};

struct Request
{
	Method method;
	std::string fileName;
	char * reqBody;
	std::map<std::string, std::string> envVars;
};

