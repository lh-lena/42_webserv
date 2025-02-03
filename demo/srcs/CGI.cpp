
#include "../includes/CGI.hpp"
#include "../includes/Server.hpp"

// CGI::CGI(const Server& server, const Location& loc) : 
// {}


void CGI::setEnvironment(const Request& request)
{
	env["AUTH_TYPE"] = std::string();
	// env["CONTENT_TYPE"] = request.contentType;
	env["CONTENT_LENGTH"] = utils::itos(request.contentLength);
	env["GATEWAY_INTERFACE"] = "CGI/1.1";
	// env["HTTP_ACCEPT_CHARSET"] = request.charset;
	// env["PATH_INFO"] = request.pathInfo; // the portion of the URI path following the script name but preceding any query data.
	env["PATH_TRANSLATED"] = std::string();
	env["QUERY_STRING"] = request.query;
	env["REMOTE_ADDR"] = std::string(); // Returns the IP address of the client that sent the request
	env["REMOTE_HOST"] = std::string(); // the fully-qualified name of the client that sent the request, or the IP address of the clien
	env["REMOTE_IDENT"] = std::string();
	env["REMOTE_USER"] = std::string();
	env["REQUEST_METHOD"] = request.method;
	env["SCRIPT_NAME"] = std::string();
	env["SERVER_NAME"] = std::string();
	env["SERVER_PORT"] = std::string();
	env["SERVER_PROTOCOL"] = request.protocol;
	env["SERVER_SOFTWARE"] = std::string();
	env["SERVER_ROOT"] = std::string();

	std::map<std::string, std::string>::iterator it = env.begin();
	envp = new char *[env.size() + 1];
	size_t i = 0;
	for (; it != env.end(); it++, i++)
	{
		envp[i] = new char[it->first.size() + it->second.size() + 2];
		std::strcpy(envp[i], it->first.c_str());
		std::strcat(envp[i], "=");
		std::strcat(envp[i], it->second.c_str());
	}

	envp[i] = NULL;
}

void	CGI::printEnvironment()
{
    std::cout << "CGI::printEnvironment()" << envp << std::endl;

	size_t j = 0;
	while (envp[j] != NULL)
	{
		printf("%s\n", envp[j]);
		j++;
	}
}
