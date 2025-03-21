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

# define MAX_RESP_SIZE 150000

class CGI
{
public:
	CGI();
	~CGI();

	void		printEnvironment();
	void		setEnvironment(const Request & request);
	void		setChildProcess(Request& request);
	void		readResponse();
	std::string	executeCGI(Request & request);
	void		setInterpreter(const std::string& str);
	void		setExecutable(const std::string& str);
	void		setUploadDir(const std::string& str);
	pid_t		getProcessId();
	int			getIfd();
	int			getOfd();
	std::string getCGIResponce();
	
	private:

	pid_t		_pid;
	int			_fds[2];
	std::string	_cgi_responce; //CGI output from the pipe

	std::vector<std::pair<std::string, std::string> >	_env;
	std::vector<char*>			envp;
	std::vector<std::string>	required_vars;
	std::vector<std::string>	remove_vars;
	std::string					interpreter;
	std::string					upload_path;
	std::string					executable;
	void	cleanEnvironment();
	void	addEnvField(const std::string& key, const std::string& value);
};

#endif /* CGIHANDLER_HPP */
