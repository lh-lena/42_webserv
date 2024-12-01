#include "../includes/ParseConfig.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

ParseConfig::ParseConfig(char **envp) : _envp(envp)
{
	this->setHttpDirective("server");
	this->setHttpDirective("error_log");
	this->setHttpDirective("client_max_body_size");
	this->setServerDirective("root");
	this->setServerDirective("index");
	this->setServerDirective("listen");
	this->setServerDirective("location");
	this->setServerDirective("error_page");
	this->setServerDirective("server_name");
	this->setServerDirective("client_max_body_size");
	this->setLocationDirective("root");
	this->setLocationDirective("index");
	this->setLocationDirective("error_page");
	this->setLocationDirective("root");
	this->setLocationDirective("root");
	
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

void ParseConfig::setHttpDirective(const std::string &directive)
{
	this->_http_directives.push_back(directive);
}

void ParseConfig::setServerDirective(const std::string &directive)
{
	this->_server_directives.push_back(directive);
}

void ParseConfig::setLocationDirective(const std::string &directive)
{
	this->_location_directives.push_back(directive);
}

std::string ParseConfig::getEnvValue(char **envp, std::string &variable)
{
	std::string value = "";
	for (int i = 0; envp[i] != NULL; i++)
	{
		if (std::strncmp(envp[i], variable.c_str(), variable.length()) == 0)
		{
			value = envp[i] + variable.length() + 1;
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

void ParseConfig::parseConfigFile(std::string& filePath)
{
	if (ParseConfig::isDirectory(filePath))
		throw ParseException("[emerg] :  open() " + filePath + " failed (Is a directory)");

	std::ifstream conf_file(filePath.c_str());

	if (!conf_file.is_open())
		throw ParseException("[emerg] : open() " + filePath + " failed (" + std::strerror(errno) + ")");
}

/* ************************************************************************** */