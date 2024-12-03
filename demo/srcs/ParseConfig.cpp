#include "../includes/ParseConfig.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

ParseConfig::ParseConfig(std::string file_path, char **envp) : _conf_file_path("serv.conf"), _envp(envp)
{
	if (!file_path.empty())
		_conf_file_path = file_path;
	this->setGlobalDirective("worker_connections", false);
	this->setGlobalDirective("http", true);
	this->setHttpDirective("server", false);
	this->setHttpDirective("error_log", false);
	this->setHttpDirective("client_max_body_size", false);
	this->setServerDirective("root", false);
	this->setServerDirective("index", false);
	this->setServerDirective("listen", true);
	this->setServerDirective("timeout", false);
	this->setServerDirective("location", false);
	this->setServerDirective("error_page", false);
	this->setServerDirective("server_name", false);
	this->setServerDirective("client_max_body_size", false);
	this->setLocationDirective("root", false);
	this->setLocationDirective("index", false);
	this->setLocationDirective("return", false);
	this->setLocationDirective("timeout", false);
	this->setLocationDirective("autoindex", false);
	this->setLocationDirective("error_page", false);
	this->setLocationDirective("allowed_methods", false);
	this->setLocationDirective("upload_directory", false);
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

/* void ParseConfig::parseConfigContent( void )
{
	std::list<std::string>::iterator it = _conf_content.begin();
	std::list<std::string>::iterator it_end = _conf_content.end();
	Server serv;
	while (it != it_end)
	{
		std::map<std::string, bool>::iterator it_gl = _global_directives.begin();
		for (; it_gl != _global_directives.end(); it_gl++)
		{
			if (it_gl->second && (*it).compare(0, (*it).length(), it_gl->first) == 0)
			{
				exceptTocken(&_conf_content, it_gl->first);
			}
		}
	}
} */

int	ParseConfig::exceptTocken(std::list<std::string> *src, std::string tocken)
{
	if ((*src).front() == tocken)
	{
		(*src).pop_front();
		return 1;
	}
	return 0;
}

void	ParseConfig::printConfigContent( void )
{
	std::list<std::string>::iterator it;
	it = _conf_content.begin();
	while (it != _conf_content.end())
	{
		std::cout << *it << std::endl;
		it++;
	}
}

void ParseConfig::setGlobalDirective(const std::string &directive, bool required)
{
	_global_directives[directive] = required;
}

void ParseConfig::setHttpDirective(const std::string &directive, bool required)
{
	_http_directives[directive] = required;
}

void ParseConfig::setServerDirective(const std::string &directive, bool required)
{
	_server_directives[directive] = required;
}

void ParseConfig::setLocationDirective(const std::string &directive, bool required)
{
	_location_directives[directive] = required;
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
