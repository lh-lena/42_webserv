
#include "../includes/CGI.hpp"
#include "../includes/Server.hpp"
#include <signal.h>

CGI::CGI(void) :
	interpreter(std::string()),
	upload_path(std::string()),
	executable(std::string())
{
	required_vars.push_back("CONTENT_TYPE");
	required_vars.push_back("CONTENT_LENGTH");
	required_vars.push_back("REQUEST_METHOD");
	required_vars.push_back("REQUEST_URI");
	required_vars.push_back("QUERY_STRING");
	required_vars.push_back("PATH_INFO");
	required_vars.push_back("PATH_TRANSLATED");
	required_vars.push_back("SCRIPT_NAME");
	required_vars.push_back("SERVER_PROTOCOL");
	required_vars.push_back("REMOTE_PORT");
	required_vars.push_back("REMOTE_ADDR");
	required_vars.push_back("REMOTE_IDENT");
	required_vars.push_back("REMOTE_USER");
	remove_vars.push_back("Auth-Scheme");
	remove_vars.push_back("Authorization");
	remove_vars.push_back("Connection");
	remove_vars.push_back("Host");
}

CGI::~CGI(void)
{
	for (size_t i = 0; i < envp.size(); i++)
		delete[] envp[i];
}

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
		if (!utils::is_str_in_vector(it->first, required_vars))
		{
			std::string key = "HTTP_" + it->first;
			it->first = key;
		}
		it++;
	}

	tmp_val = !request.getHeader("Auth-Scheme").empty() ? request.getHeader("Authorization") : std::string();
	addEnvField("AUTH_TYPE", tmp_val);
	addEnvField("GATEWAY_INTERFACE", "CGI/1.1");
	addEnvField("REDIRECT_STATUS", "1");
	addEnvField("DISPLAY", ":0");
	addEnvField("DBUS_SESSION_BUS_ADDRESS", "unix:path=/run/user/101457/bus");
	/** Filesystem- (not document root-) based path to the current script, after the server has done any virtual-to-real mapping. */
	addEnvField("REMOTE_HOST", std::string());
	/** The absolute pathname of the currently executing script */
	addEnvField("SCRIPT_FILENAME", request.getFullPath());
	addEnvField("SERVER_ROOT", std::string());
	/** The document root directory under which the current script is executing, as defined in the server's configuration file */
	addEnvField("DOCUMENT_ROOT", std::string());
	addEnvField("SERVER_SOFTWARE", "Webserv/1.0");
	/** The name of the server host under which the current script is executing */
	addEnvField("SERVER_NAME", utils::substr_before_rdel(request.getHeader("Host"), ":"));
	addEnvField("SERVER_PORT", utils::substr_after_rdel(request.getHeader("Host"), ":"));
	addEnvField("UPLOAD_PATH", upload_path);

	std::vector<std::pair<std::string, std::string> >::const_iterator it2 = _env.begin();
	for (; it2 != _env.end(); ++it2)
	{
		std::string el =  it2->first + "=" + it2->second;
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
	for(; it != envp.end() && *it != NULL; ++it)
	{
		std::cout << *it << std::endl;
	}
}

void	CGI::setChildProcess(Request& request)
{
	int in_fds[2];
	int out_fds[2];
	pipe(in_fds);
	pipe(out_fds);

	_fds[0] = out_fds[0]; //POLLIN : read from the pipe
	_fds[1] = in_fds[1]; //POLLOUT : write to the pipe

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

		if (execve(arg, nll, envp.data()) < 0)
		{
			std::cerr << RED << "[ERROR] : execve() failed\n" << RESET;
			write(STDOUT_FILENO, "Status: 500\n\n", 15);
			exit(0);
		}
	}
	_pid = pid;
	close(in_fds[0]);
	close(out_fds[1]);

	std::string b = request.getBody();
	char *str = (char *)b.c_str();
	if (str[0] != '\0')
	{
		int res = write(in_fds[1], str, b.length());
		if (res == -1)
			std::cerr << "Error: write() failed" << std::endl;
		if (res == 0)
			std::cerr << "Error: write(): no data sent" << std::endl;
	}
	close(in_fds[1]);
	_timer = time(NULL);
}

void	CGI::readResponse()
{
	char buf[MAX_RESP_SIZE];
	int res = 0;

	memset(buf, 0, sizeof(buf));
	res = read(_fds[0], &buf, sizeof(buf) - 1);
	if (res == -1)
	{
		std::cerr << "read() CGI response failed";
		_cgi_responce = "Content-Type: text/plain\nStatus: 500 no data received from CGI\n\n";
	}
	if (res == 0)
	{
		std::cerr << "CGI return empty";
		_cgi_responce = "Content-Type: text/plain\nStatus: 500 no data received from CGI\n\n";
	}
	if (res > 0)
		_cgi_responce.append(buf, res);
	if (res == MAX_RESP_SIZE - 1)
	{
		if (read(_fds[0], &buf, 1) == 1)
		{
			if (kill(_pid, SIGKILL) == -1)
				std::cerr << "Error: failed to kill CGI child process pid " << _pid << std::endl;
			std::cerr << "Error: data received from CGI is too big" << std::endl;
			_cgi_responce = "Content-Type: text/plain\nStatus: 413 data received from CGI is too big\n\n";
		}
	}

	close(_fds[0]);
	waitpid(_pid, NULL, 0);
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

pid_t	CGI::getProcessId()
{
	return _pid;
}

int		CGI::getIfd()
{
	return _fds[0];
}

int		CGI::getOfd()
{
	return _fds[1];
}

std::string CGI::getCGIResponce()
{
	return _cgi_responce;
}

time_t	CGI::getTimer()
{
	return _timer;
}
