
#include "../includes/CGI.hpp"
#include "../includes/Server.hpp"
// #include "../includes/Request.hpp"

// CGI::CGI(const Server& server, const Location& loc) :
// {}

CGI::CGI(void) :
	interpreter(std::string()),
	upload_path(std::string()),
	executable(std::string())
{
	requered_vars.push_back("CONTENT_TYPE");
	requered_vars.push_back("CONTENT_LENGTH");
	requered_vars.push_back("REQUEST_METHOD");
	requered_vars.push_back("REQUEST_URI");
	requered_vars.push_back("QUERY_STRING");
	requered_vars.push_back("SERVER_PROTOCOL");

	remove_vars.push_back("Auth-Scheme");
	remove_vars.push_back("Authorization");
	remove_vars.push_back("Connection");
}

CGI::~CGI(void)
{}

void CGI::setEnvironment(const Request& request)
{
	std::string tmp_val = "";
	_env = request.getHeaders();

	std::vector<std::pair<std::string, std::string> >::iterator it = _env.begin();
	for(; it != _env.end();)
	{
		if (utils::is_str_in_vector(it->first, remove_vars))
		{
			it = _env.erase(it);
			continue;
		}
		it->first = utils::str_toupper(it->first);
		std::replace(it->first.begin(), it->first.end(), '-', '_');
		if (!utils::is_str_in_vector(it->first, requered_vars))
		{
			std::string key = "HTTP_" + it->first;
			it->first = key;
		}
		it++;
	}
	std::cerr << "INFO 4" << std::endl;
	tmp_val = !request.getHeader("Auth-Scheme").empty() ? request.getHeader("Authorization") : std::string();
	addEnvField("AUTH_TYPE", tmp_val);
	addEnvField("GATEWAY_INTERFACE", "CGI/1.1");
	// addEnvField("REDIRECT_STATUS", "200"); //??
	addEnvField("PATH_INFO", "/"); // the portion of the URI path following the script name but preceding any query data.
	addEnvField("PATH_TRANSLATED", std::string());
	addEnvField("REMOTE_ADDR", std::string());
	addEnvField("REMOTE_HOST", std::string());
	addEnvField("SERVER_PORT", std::string());
	addEnvField("REMOTE_IDENT", request.getHeader("Authorization"));
	addEnvField("REMOTE_USER", request.getHeader("Authorization"));
	// addEnvField("REQUEST_URI", request.getURI() + request.getQueryString()); //?
	addEnvField("SCRIPT_NAME", request.getFullPath());
	addEnvField("SCRIPT_FILENAME", request.getFullPath());
	addEnvField("SERVER_ROOT", std::string());
	addEnvField("SERVER_SOFTWARE", "42-webserv/1.0");
	addEnvField("SERVER_NAME", utils::substr_before_rdel(request.getHeader("host"), ":"));
	addEnvField("SERVER_PORT", utils::substr_after_rdel(request.getHeader("host"), ":"));
	addEnvField("UPLOAD_PATH", upload_path);
	// addEnvField("WEBTOP_USER", request.getHeader("Cookie")); // The user name of the user who is logged in.
	std::cerr << "INFO 5" << std::endl;
	std::vector<std::pair<std::string, std::string> >::const_iterator it2 = _env.begin();
	for (; it2 != _env.end(); ++it2)
	{
		std::string el =  it2->first + "=" + it2->second;
		char *cStr = new char[el.length() + 1];
		std::strcpy(cStr, el.c_str());
		envp.push_back(cStr);
	}
	std::cerr << "INFO 6" << std::endl;
	envp.push_back(NULL);
	printEnvironment();
}

void	CGI::cleanEnvironment()
{
	for (size_t i = 0; i < envp.size(); i++)
		delete[] envp[i];
}

void	CGI::printEnvironment()
{
	std::cout << "CGI::printEnvironment()" << std::endl;

	std::vector<char*>::iterator it = envp.begin();
	for(; it != envp.end() && *it != NULL; ++it)
	{
		std::cout << *it << std::endl;
	}
}

std::string		CGI::executeCGI(Request& request)
{
	int in_fds[2];
	int out_fds[2];
	pipe(in_fds);
	pipe(out_fds);

	// add to ServerController::_pfds POLLIN
	fds[0] = out_fds[0];
	// add to ServerController::_pfds POLLOUT
	fds[1] = in_fds[1];
	//request.connection->cgi_fds[0] = out_fds[0];
	//request.connection->cgi_fds[1] = in_fds[1];

	pid_t pid = fork();
	if (pid == 0)
	{
		dup2(in_fds[0], STDIN_FILENO);
		close(in_fds[0]);
		close(in_fds[1]);
		dup2(out_fds[1], STDOUT_FILENO);
		close(out_fds[0]);
		close(out_fds[1]);
		char * const * nll = NULL;
		char *arg = {(char *)executable.c_str()};
		std::cerr << "executable CGI: " << executable << std::endl;
		if (execve(arg, nll, envp.data()) < 0)
		{
			std::cerr << RED << "Error: execve() failed\n" << RESET;
			write(STDOUT_FILENO, "Status: 500\n\n", 15);
			exit(0);
		}
	}
	close(in_fds[0]);
	close(out_fds[1]);
	std::string b = request.getBody();
	char *str = (char *)b.c_str();
	if (str[0] != '\0')
		write(in_fds[1], str, b.length());
	close(in_fds[1]);
	//request.connection->cgi_fds[1] = -1;

	// check for timeout before reading from the pipe

	std::string respn; //result message returned from execve()
	char buf[1500];
	int res = 0;
	do
	{
		memset(buf, 0, sizeof(buf));
		res = read(out_fds[0], &buf, 1500);
		if (res > 0)
			respn.append(buf);

	} while (res == 1500);

	close(out_fds[0]);
	//request.connection->cgi_fds[0] = -1;

	waitpid(pid, NULL, 0); // kill child process in case of timeout;
	std::cerr << GREEN << "RESPONSE CGI :" << respn << RESET << std::endl;
	cleanEnvironment();
	return respn;
}

void		CGI::setInterpreter(const std::string& str)
{
	interpreter = str;
}

void		CGI::setExecutable(const std::string& str)
{
	executable = str;
}

void		CGI::setUploadDir(const std::string& str)
{
	upload_path = str;
}

void		CGI::addEnvField(const std::string& key, const std::string& value)
{
	_env.push_back(std::make_pair(key, value));
}
