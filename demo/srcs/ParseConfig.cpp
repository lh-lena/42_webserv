#include "../includes/ParseConfig.hpp"

/*
** ------------------------------ DECLARATION ---------------------------------
*/

template void	ParseConfig::handleWorkCont(const std::string&, Server*);
template void	ParseConfig::handleWorkCont(const std::string&, Location*);

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

ParseConfig::ParseConfig(std::string file_path, char **envp) : _conf_file_path("serv.conf"), _envp(envp)
{
	if (!file_path.empty())
		_conf_file_path = file_path;
	this->setGlobalDirective("worker_connections", &ParseConfig::handleWorkCont);
	this->setGlobalDirective("http", &ParseConfig::handleHttpBlock);
	this->setHttpDirective("server", &ParseConfig::handleServerBlock);
	// this->setHttpDirective("error_log", false);
	// this->setHttpDirective("client_max_body_size", false);
	// this->setServerDirective("root", false);
	// this->setServerDirective("index", false);
	// this->setServerDirective("listen", true);
	// this->setServerDirective("timeout", false);
	// this->setServerDirective("location", false);
	// this->setServerDirective("error_page", false);
	// this->setServerDirective("server_name", false);
	// this->setServerDirective("client_max_body_size", false);
	// this->setLocationDirective("root", false);
	// this->setLocationDirective("index", false);
	// this->setLocationDirective("return", false);
	// this->setLocationDirective("timeout", false);
	// this->setLocationDirective("autoindex", false);
	// this->setLocationDirective("error_page", false);
	// this->setLocationDirective("allowed_methods", false);
	// this->setLocationDirective("upload_directory", false);
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

void ParseConfig::readFileContent( void )
{
	if (ParseConfig::isDirectory(_conf_file_path))
		throw ParseException("[emerg] :  open() " + _conf_file_path + " failed (Is a directory)");

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
			if (word == "#" || word[0] == '#')
				break;
			_conf_content.push_back(word);
		}
	}
	conf_file.close();
}

void ParseConfig::parseConfigContent( void )
{
	std::list<std::string>::iterator it = _conf_content.begin();
	std::list<std::string>::iterator it_end = _conf_content.end();
	std::string directive;
	std::string value;
	Server server;
	while (it != it_end)
	{
		directive = *it;
		if (_global_directives.find(directive) != _global_directives.end())
		{
			exceptTocken(&_conf_content, directive);
			it++;
			value = *it;
			while (it != it_end)
			{
				// value = value.substr(0, value.size()-1);
				if (value.find(";") != std::string::npos)
					;
				DirectiveHandler handler = _global_directives[directive];
				(this->*handler)(value, &server);
			}
		}
		else
		{
			std::cerr << "Unknown directive: " << directive << std::endl;
		}
	}
}

template<typename T> void	ParseConfig::handleWorkCont(const std::string& value, T* instance)
{
	std::cout << "Generic handler for directive: " << value << std::endl;
	instance->_
}

template<typename T> void	ParseConfig::handleHttpBlock(const std::string& value, T* instance)
{
	std::cout << "Generic handler for directive: " << value << std::endl;
}

template<typename T> void	ParseConfig::handleServerBlock(const std::string& value, T* instance)
{
	std::cout << "Generic handler for directive: " << value << std::endl;
}

int	ParseConfig::exceptTocken(std::list<std::string> *src, std::string tocken)
{
	if ((*src).front() == tocken)
	{
		(*src).pop_front();
		return 1;
	}
	return 0;
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
	std::cout << path << std::endl;
	struct stat path_stat;
	if (stat(path.c_str(), &path_stat) != 0)
	{
		std::cerr << "[ERROR]: Accessing path failed: " << std::strerror(errno) << std::endl;
		return false;
	}
	return S_ISDIR(path_stat.st_mode);
}

void ParseConfig::setGlobalDirective(const std::string &directive, DirectiveHandler handler)
{
	_global_directives[directive] = handler;
}

void ParseConfig::setHttpDirective(const std::string &directive, DirectiveHandler handler)
{
	_http_directives[directive] = handler;
}

void ParseConfig::setServerDirective(const std::string &directive, DirectiveHandler handler)
{
	_server_directives[directive] = handler;
}

void ParseConfig::setLocationDirective(const std::string &directive, DirectiveHandler handler)
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
		config.printConfigContent();
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}
}
