#ifndef CGIHANDLER_HPP

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

class CGIHandler
{
public:
	CGIHandler(const Server& server, const Location& loc, const Request& request, Response& response);
	~CGIHandler();

	void	setupVariables(const Request& request);
	void	setEnvironment(const Request& request);
	void	createChildProcess(Response& response, const Request& request);
	void	executeCGI(Response& response);

	std::map<std::string, std::string> env;
private:
};

#endif /* CGIHANDLER_HPP */