
#include "../includes/CGI.hpp"
#include "../includes/Server.hpp"
// #include "../includes/Request.hpp"

// CGI::CGI(const Server& server, const Location& loc) :
// {}

CGI::CGI(void) :
	interpreter(std::string()),
	upload_dir(std::string()),
	executable(std::string())
{}

CGI::~CGI(void)
{}

void CGI::setEnvironment(const Request& request)
{
	env["AUTH_TYPE"] = !request.getHeader("Auth-Scheme").empty() ? request.getHeader("Authorization") : std::string();
	env["CONTENT_TYPE"] = request.getHeader("Content-Type");
	env["CONTENT_LENGTH"] = request.getHeader("Content-Length");
	env["GATEWAY_INTERFACE"] = "CGI/1.1";
	env["REDIRECT_STATUS"] = "200";
	env["HTTP_ACCEPT_CHARSET"] = request.getHeader("Accept-Charset");
	env["PATH_INFO"] = "/"; // the portion of the URI path following the script name but preceding any query data.
	env["PATH_TRANSLATED"] = std::string();
	env["QUERY_STRING"] = request.getQueryString();
	env["REMOTE_ADDR"] = std::string(); // Returns the IP address of the client that sent the request
	env["REMOTE_HOST"] = std::string(); // the fully-qualified name of the client that sent the request, or the IP address of the clien
	env["REMOTE_IDENT"] = request.getHeader("Authorization");
	env["REMOTE_USER"] = request.getHeader("Authorization");
	env["REQUEST_METHOD"] = request.getMethod();
	env["REQUEST_URI"] = request.getURI() + request.getQueryString();
	env["SCRIPT_NAME"] = request.getFullPath();
	env["SCRIPT_FILENAME"] = request.getFullPath();
	env["SERVER_NAME"] = request.getHeader("Hostname");
	env["SERVER_PORT"] = std::string();
	env["SERVER_PROTOCOL"] = request.getProtocol();
	env["SERVER_SOFTWARE"] = "42-server/1.0";
	env["SERVER_ROOT"] = std::string();
	env["UPLOAD_DIR"] = upload_dir;
	env["HTTP_COOKIE"] = request.getHeader("Cookie");

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

void	CGI::cleanEnvironment()
{
	for (size_t i = 0; i < envp.size(); i++)
		delete[] envp[i];
}

void	CGI::printEnvironment()
{
	std::cout << "CGI::printEnvironment()" << std::endl;

	std::vector<char*>::iterator it = envp.begin();
	for(; it != envp.end(); it++)
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
	// std::cerr << GREEN << "RESPONSE CGI :" << respn << RESET << std::endl;
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
	upload_dir = str;
}
