#include "../includes/ParseConfig.hpp"

/*
** ------------------------------ DECLARATION ---------------------------------
*/

// template void	ParseConfig::handleErrorPage<Server>(const std::string&, Server*);
template void	ParseConfig::handleIndex<Server>(const std::string&, Server*);
template void	ParseConfig::handleRoot<Server>(const std::string&, Server*);
// template void	ParseConfig::handleErrorPage<Location>(const std::string&, Location*);
template void	ParseConfig::handleIndex<Location>(const std::string&, Location*);
template void	ParseConfig::handleRoot<Location>(const std::string&, Location*);

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

ParseConfig::ParseConfig(std::string file_path, char **envp) : _conf_file_path("serv.conf"), _envp(envp)
{
	if (!file_path.empty())
		_conf_file_path = file_path;
	block_dir["http"] = true;
	block_dir["server"] = true;
	block_dir["location"] = true;
	allowed_methods["GET"] = true;
	allowed_methods["POST"] = true;
	allowed_methods["DELETE"] = true;
	this->setGlobalDirective("worker_connections", &ParseConfig::handleWorkCont);
	this->setGlobalDirective("http", &ParseConfig::handleHttpBlock);
	this->setHttpDirective("server", &ParseConfig::handleServerBlock);
	this->setHttpDirective("error_log", &ParseConfig::handleErrorLog);
	this->setHttpDirective("client_max_body_size", &ParseConfig::handleClientBodySize);
	this->setServerDirective("root", &ParseConfig::handleRoot);
	this->setServerDirective("index", &ParseConfig::handleIndex);
	this->setServerDirective("listen", &ParseConfig::handleListen);
	// this->setServerDirective("error_page", &ParseConfig::handleErrorPage);
	this->setLocationDirective("location", &ParseConfig::handleLocationBlock);
	this->setServerDirective("location", &ParseConfig::handleServerBlock);
	this->setServerDirective("server_name", &ParseConfig::handleServerName);
	this->setServerDirective("client_max_body_size", &ParseConfig::handleClientBodySize);
	this->setLocationDirective("root", &ParseConfig::handleRoot);
	this->setLocationDirective("path", &ParseConfig::handlePath);
	this->setLocationDirective("index", &ParseConfig::handleIndex);
	// this->setLocationDirective("return", &ParseConfig::handleRedirect);
	this->setLocationDirective("autoindex", &ParseConfig::handleAutoindex);
	// this->setLocationDirective("error_page", &ParseConfig::handleErrorPage);
	this->setLocationDirective("allowed_methods", &ParseConfig::handleAllowedMethods);
	this->setLocationDirective("upload_directory", &ParseConfig::handleUploadDir);
	this->setLocationDirective("cgi_extension", &ParseConfig::handleCgiExtension);
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

ParseConfig::~ParseConfig() {}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

/*
** --------------------------------- EXCEPTION -------------------------------
*/

ParseConfig::ParseException::ParseException(const std::string &message) : _msg(message) {};

const char *ParseConfig::ParseException::what() const throw()
{
	return (_msg.c_str());
}

ParseConfig::ParseException::~ParseException() throw() {};


/*
** --------------------------------- METHODS ----------------------------------
*/

void		ParseConfig::readFileContent( void )
{
	if (ParseConfig::isDirectory(_conf_file_path))
		throw ParseException("[emerg] : open() " + _conf_file_path + " failed (Is a directory)");

	std::ifstream conf_file(_conf_file_path.c_str());

	if (!conf_file.is_open())
		throw ParseException("[emerg] : open() " + _conf_file_path + " failed (" + std::strerror(errno) + ")");

	std::string line;
	while (std::getline(conf_file, line))
	{
		std::istringstream	stream(line);
		std::string			word;
		while (stream >> word)
		{
			size_t last_char_pos = word.size() - 1;
			char last_char = word[last_char_pos];

			if (word.size() - 1 > 0 && (last_char == ';' || last_char  == '{' || last_char == '}'))
			{
				std::string symbol(1, last_char);
				word.erase(last_char_pos);
				_conf_content.push_back(word);
				_conf_content.push_back(symbol);
			}
			else if (word == "#" || word[0] == '#')
				break;
			else
				_conf_content.push_back(word);
		}
	}
	conf_file.close();
}

/**
 * The function `parseConfigContent` parses configuration content by reading directives and values,
 * handling them accordingly, and catching any exceptions that may occur.
 */
void 		ParseConfig::parseConfigContent( void )
{
	std::string	directive;
	std::string	value;
	Server		server;

	try
	{
		while (!_conf_content.empty())
		{
			directive = getToken(&_conf_content);
			if (_global_directives.find(directive) == _global_directives.end())
			{
				std::cerr << "[ERROR] parseConfigContent Unknown directive: " << directive << std::endl;
				return ;
			}
			exceptTocken(&_conf_content, directive);
			value = getToken(&_conf_content);
			DirectiveServerHandler serv_handler = _global_directives[directive];
			(this->*serv_handler)(value, (Server*)&server);
			if (block_dir.find(directive) == block_dir.end())
			{
				exceptTocken(&_conf_content, value);
				exceptTocken(&_conf_content, ";");
			}
		}
		std::cout << _serverControler << std::endl;
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
}

void		ParseConfig::handleHttpBlock(const std::string& value, Server* instance)
{
	std::string directive;
	std::string val;

	try
	{
		exceptTocken(&_conf_content, "{");
		while (!_conf_content.empty())
		{
			directive = getToken(&_conf_content);
			if (directive == "}")
				break;
			if (_http_directives.find(directive) == _http_directives.end())
				throw ParseException("[emerg] unknown directive " + directive + " in " + _conf_file_path);
			exceptTocken(&_conf_content, directive);
			if (block_dir.find(directive) != block_dir.end())
			{
				Server server(*instance);
				DirectiveServerHandler serv_handler = _http_directives[directive];
				(this->*serv_handler)(val, (Server*)&server);
				_serverControler.setServer(server);
				continue;
			}
			val = getToken(&_conf_content);
			while (val != ";")
			{
				DirectiveServerHandler serv_handler = _http_directives[directive];
				(this->*serv_handler)(val, instance);
				exceptTocken(&_conf_content, val);
				val = getToken(&_conf_content);
			}
			exceptTocken(&_conf_content, ";");
		}
		exceptTocken(&_conf_content, "}");
	}
	catch(const std::exception& e)
	{
		throw ParseException(e.what());
	}
}

void		ParseConfig::handleServerBlock(const std::string& value, Server* instance)
{
	try
	{
		exceptTocken(&_conf_content, "{");
		while (!_conf_content.empty())
		{
			std::string directive;
			std::string val;
			directive = getToken(&_conf_content);
			if (directive == "}")
				break;
			if (_server_directives.find(directive) == _server_directives.end())
				throw ParseException("[emerg] unknown directive " + directive + " in " + _conf_file_path);
			exceptTocken(&_conf_content, directive);
			val = getToken(&_conf_content);
			if (block_dir.find(directive) != block_dir.end())
			{
				Location location;
				DirectiveLocationHandler loc_handler = _location_directives[directive];
				(this->*loc_handler)(val, (Location*)&location);
				std::cout << "location: " << location << std::endl;
				instance->setLocation(location);
				continue;
			}
			while (val != ";")
			{
				DirectiveServerHandler serv_handler = _server_directives[directive];
				(this->*serv_handler)(val, instance);
				exceptTocken(&_conf_content, val);
				val = getToken(&_conf_content);
				if (val == ";")
					break;
			}
			exceptTocken(&_conf_content, ";");
		}
		exceptTocken(&_conf_content, "}");
	}
	catch(const std::exception& e)
	{
		throw ParseException(e.what());
	}
}

void		ParseConfig::handleLocationBlock(const std::string& value, Location* instance)
{
	DirectiveLocationHandler loc_handler = _location_directives["path"];
	(this->*loc_handler)(value, instance);
	exceptTocken(&_conf_content, value);
	try
	{
		exceptTocken(&_conf_content, "{");
		while (!_conf_content.empty())
		{
			std::string directive;
			std::string val;
			directive = getToken(&_conf_content);
			if (directive == "}")
				break;
			if (_location_directives.find(directive) == _location_directives.end())
				throw ParseException("[emerg] unknown directive " + directive + " in " + _conf_file_path);
			exceptTocken(&_conf_content, directive);
			val = getToken(&_conf_content);
			while (val != ";")
			{
				DirectiveLocationHandler loc_handler = _location_directives[directive];
				(this->*loc_handler)(val, instance);
				exceptTocken(&_conf_content, val);
				val = getToken(&_conf_content);
				if (val == ";")
					break;
			}
			exceptTocken(&_conf_content, ";");
		}
		exceptTocken(&_conf_content, "}");
	}
	catch(const std::exception& e)
	{
		throw ParseException(e.what());
	}
}

void		ParseConfig::handleWorkCont(const std::string& value, Server* instance)
{
	int val = 0;

	if (!is_digits(value))
		throw ParseConfig::ParseException("[emerg] : directive \"worker_connections\" required only digits in " + _conf_file_path);
	std::istringstream iss(value);
	iss >> val;
	if (iss.bad() || val <= 0)
		throw ParseConfig::ParseException("[emerg] : directive \"worker_connections\" required a positive number in " + _conf_file_path);

	instance->setWorkCont(val);
}

template<typename T> void	ParseConfig::handleRoot(const std::string& value, T* instance)
{
	instance->setRoot(value);
}

void		ParseConfig::handlePath(const std::string& value, Location* instance)
{
	instance->setPath(value);
}

void		ParseConfig::handleErrorLog(const std::string& value, Server* instance)
{
	instance->setErrorLog(value);
}

void		ParseConfig::handleClientBodySize(const std::string& value, Server* instance)
{
	int val = 0;
	std::string unit = value.substr(value.length() - 1);
	std::string nbr = value.substr(0, value.length() - 1);
	if (!is_digits(nbr))
		throw ParseException("[emerg] : directive \"client_max_body_size\" required only digits in " + _conf_file_path);
	std::istringstream iss(nbr);
	iss >> val;
	if (iss.bad() || val <= 0)
		throw ParseException("[emerg] : directive \"client_max_body_size\" required a positive number in " + _conf_file_path);
	if (val > 100)
		throw ParseException("[emerg] : directive \"client_max_body_size\" limited size up to 100 Megabytes in " + _conf_file_path);
	if (unit != "M")
		throw ParseException("[emerg] : directive \"client_max_body_size\" required a number's unit in Megabytes in " + _conf_file_path);
	val = val * 1024 * 1024;
	instance->setClientMaxBody(val);
}
template<typename T> void	ParseConfig::handleIndex(const std::string& value, T* instance)
{
	instance->setIndex(value);
}

void		ParseConfig::handleListen(const std::string& value, Server* instance)
{
	size_t pos = value.find(":");
	if (pos == std::string::npos)
	{
		instance->setPort(value);
		return;
	}
	std::string host = value.substr(0, pos);
	if	(host[0] == '{')
		host = getEnvValue(_envp, host);
	instance->setHost(host);
	std::string port = value.substr(pos + 1);
	if (port[0] == '{')
		port = getEnvValue(_envp, port);
	instance->setPort(port);
}


void		ParseConfig::handleServerName(const std::string& value, Server* instance)
{
	instance->setServerName(value);
}

void		ParseConfig::handleAutoindex(const std::string& value, Location* instance)
{
	if (value == "on")
		instance->setAutoindex(true);
	else if (value == "off")
		instance->setAutoindex(false);
	else
		throw ParseException("[emerg] : directive \"autoindex\" misses 'on' or 'off' statement in " + _conf_file_path);
}

void		ParseConfig::handleAllowedMethods(const std::string& value, Location* instance)
{
	if (this->allowed_methods.find(value) != this->allowed_methods.end())
	{
		instance->setAllowedMethod(value);
	}
	else
	{
		throw ParseException("[emerg] : method \"" + value + "\" not allowed in " + _conf_file_path);
	}
}
/** TODO: */
/* void	ParseConfig::handleRedirect(const std::string& value, Location* instance)
{

} */

/** TODO: */
/* template<typename T> void	ParseConfig::handleErrorPage(const std::string& value, T* instance)
{

} */


void		ParseConfig::handleUploadDir(const std::string& value, Location* instance)
{
	instance->setUploadDir(value);
}

void		ParseConfig::handleCgiExtension(const std::string& value, Location* instance)
{
	instance->setCgiExtension(value);
}

/**
 * The function `exceptTocken` checks if the front element of a list matches a given token and prints a
 * message accordingly.
 * 
 * @param src The `src` parameter is a pointer to a `std::list` of `std::string` objects. It is used to
 * store a list of strings that are being parsed or processed.
 * @param tocken The `tocken` parameter in the `exceptTocken` function is a `std::string` that
 * represents the token that is expected to be at the front of the list `src`. The function checks if
 * the list is not empty and if the front element of the list matches the `
 * 
 * @return The function `exceptTocken` returns an integer value. If the condition `!src->empty() &&
 * src->front() == tocken` is true, it returns 1 after printing the message "[exceptTocken] : "
 * followed by the front element of the list `src`. If the condition is false, it prints "[emerg] :
 * unexpected " followed by the file name`
 */
int		ParseConfig::exceptTocken(std::list<std::string> *src, std::string tocken)
{
	if (!src->empty() && src->front() == tocken)
	{
		std::cout << "[exceptTocken] : " << (*src).front() << std::endl;
		src->pop_front();
		return 1;
	}
	else
		std::cout << "[emerg] : unexpected \"" << tocken << "\" in " << _conf_file_path << std::endl;
	return 0;
}


/**
 * The function `processEnvVar` parses a string input to extract and return the value of an environment
 * variable specified within `${}`.
 * 
 * @param input The `processEnvVar` function takes a string `input` as a parameter. This function is
 * designed to extract and process environment variables enclosed within `${}` in the input string.
 * 
 * @return The `processEnvVar` function returns the value of an environment variable specified within
 * `${}` in the input string. If the input string contains `${}` with a valid variable name inside, it
 * will return the value of that environment variable. Otherwise, it will throw a `ParseException` with
 * an error message indicating that the input is invalid.
 */
std::string ParseConfig::processEnvVar(const std::string &input)
{
	size_t start = input.find("${");
	size_t end = input.find("}");

	if (start != std::string::npos && end != std::string::npos && (start + 2) != end)
	{
		std::string var = input.substr(start + 2, end - start - 2);
		return (getEnvValue(_envp, var));
	}
	else
	{
		throw ParseException("[emerg] \"" + input + "\" invalid input in " + _conf_file_path);
	}
}

std::string ParseConfig::getEnvValue(char **envp, const std::string &variable)
{
	std::string value = "";
	size_t var_len = variable.length();
	for (int i = 0; envp[i] != NULL; i++)
	{
		if (std::strncmp(envp[i], variable.c_str(), var_len) == 0 && envp[i][var_len] == '=')
		{
			value = envp[i] + var_len + 1;
			break;
		}
	}
	return value;
}

bool ParseConfig::isDirectory(const std::string& path)
{
	struct stat path_stat;
	if (stat(path.c_str(), &path_stat) != 0)
	{
		std::cerr << "[ERROR]: Accessing path failed: " << std::strerror(errno) << std::endl;
		return false;
	}
	return S_ISDIR(path_stat.st_mode);
}

std::string ParseConfig::getToken(std::list<std::string> *src)
{
	if (src->empty())
		throw ParseConfig::ParseException("[emerg] : Unexpected end of configuration content in " + _conf_file_path);
	std::string token = src->front();
	return token;
}

bool	is_digits(const std::string& str)
{
    for (char c : str)
	{
        if (!std::isdigit(c))
            return false;
    }
    return true;
}


/*
** --------------------------------- ACCESSOR ---------------------------------
*/


void ParseConfig::setGlobalDirective(const std::string &directive, DirectiveServerHandler handler)
{
	_global_directives[directive] = handler;
}

void ParseConfig::setHttpDirective(const std::string &directive, DirectiveServerHandler handler)
{
	_http_directives[directive] = handler;
}

void ParseConfig::setServerDirective(const std::string &directive, DirectiveServerHandler handler)
{
	_server_directives[directive] = handler;
}

void ParseConfig::setLocationDirective(const std::string &directive, DirectiveLocationHandler handler)
{
	_location_directives[directive] = handler;
}



/* ************************************************************************** */

int main(int argc, char *argv[], char* envp[])
{
	(void)argc;
	(void)argv;

	std::string configFilePath;
	configFilePath = "serv.conf";
	ParseConfig config(configFilePath, envp);
	try
	{
		config.readFileContent();
		config.parseConfigContent();
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}
}
