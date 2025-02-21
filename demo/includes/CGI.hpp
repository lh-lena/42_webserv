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

class CGI
{
public:
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
	std::vector<std::pair<std::string, std::string> >	_env;
	std::vector<char*>					envp;
	std::vector<std::string>	required_vars;
	std::vector<std::string>	remove_vars;
	std::string					interpreter;
	std::string					upload_path;
	std::string					executable;
	void	cleanEnvironment();
	void	addEnvField(const std::string& key, const std::string& value);
};

#endif /* CGIHANDLER_HPP */
