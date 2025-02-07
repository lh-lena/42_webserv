#ifndef CGI_HPP
# define CGI_HPP

# include "Request.hpp"
# include "Response.hpp"
# include "Location.hpp"
# include "Server.hpp"
# include <iostream>
# include <map>
# include <vector>
# include <string>
# include <unistd.h>
# include <sys/types.h>
# include <sys/wait.h>

/** only template, not sure about arguments */
class CGI
{
public:
	// CGI(const Request& request) : _request(request) {};
	CGI();
	~CGI();

	void	setEnvironment(const Request& request);
	void	printEnvironment();
	void	handleRequest(const Request& request);
	void	createChildProcess(Request& request);
	void	executeCGI(Response& response);

	std::map<std::string, std::string>	env;
	std::vector<char*>					envp;
	// char**								envp;
	Request 							_request;
private:
};

#endif /* CGIHANDLER_HPP */
