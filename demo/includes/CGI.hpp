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

	void		printEnvironment();
	void		setEnvironment(const Request & request);
	std::string	executeCGI(Request & request);
	void		setInterpreter(const std::string& str);
	void		setExecutable(const std::string& str);
	void		setUploadDir(const std::string& str);

	int	conn_fd;
	int	fds[2];

private:
	std::map<std::string, std::string>	env;
	std::vector<char*>					envp;
	std::string							interpreter;
	std::string							upload_dir;
	std::string							executable;
	void	cleanEnvironment();
};

#endif /* CGIHANDLER_HPP */
