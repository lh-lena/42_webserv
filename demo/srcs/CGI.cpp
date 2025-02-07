
#include "../includes/CGI.hpp"
#include "../includes/Server.hpp"
//#include "../includes/Request.hpp"

// CGI::CGI(const Server& server, const Location& loc) :
// {}

CGI::CGI(void){}

CGI::~CGI(void)
{
	for (size_t i = 0; i < envp.size(); i++)
        delete[] envp[i];
}

/** handle request includes
 * parsing path,
 * handleing redirect,
 * setting variable,
 * calling execute function */

void	CGI::handleRequest(const Request& request)
{
	setEnvironment(request);
}

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
	for (; it != env.end(); it++)
	{
		std::string el =  it->first + "=" + it->second;
		char *cStr = new char[el.length() + 1];
		std::strcpy(cStr, el.c_str());
		envp.push_back(cStr);
	}

	envp.push_back(NULL);
}

void	CGI::printEnvironment()
{
    std::cout << "CGI::printEnvironment()" << std::endl;

	std::vector<char*>::iterator it = envp.begin();
	for(; it != envp.end(); ++it)
	{
		std::cout << *it << std::endl;
	}
}
