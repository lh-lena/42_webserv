#include "../includes/ParseConfig.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

ParseConfig::ParseConfig(char **envp) : _envp(envp)
{
	this->setGlobalDirective("http");
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
	this->setLocationDirective("return");
	this->setLocationDirective("autoindex");
	this->setLocationDirective("error_page");
	this->setLocationDirective("allowed_methods");
	this->setLocationDirective("upload_directory");
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

void ParseConfig::readFileContent(std::string& filePath)
{
	if (ParseConfig::isDirectory(filePath))
		throw ParseException("[emerg] :  open() " + filePath + " failed (Is a directory)");

	std::ifstream conf_file(filePath.c_str());

	if (!conf_file.is_open())
		throw ParseException("[emerg] : open() " + filePath + " failed (" + std::strerror(errno) + ")");
	
	std::string line;
	while (std::getline(conf_file, line))
	{
		std::istringstream stream(line);
		std::string word;
		while (stream >> word)
		{
			if (word == "#")
				break;
			_contentConfFile.push_back(word);
		}
	}
	conf_file.close();
}

void	ParseConfig::printConfigContent( void )
{
	std::vector<std::string>::iterator it;
	it = _contentConfFile.begin();
	while (it < _contentConfFile.end())
	{
		std::cout << *it << std::endl;
		it++;
	}
}

void ParseConfig::setGlobalDirective(const std::string &directive)
{
	this->_global_directives.push_back(directive);
}

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
	ParseConfig config(envp);
	try
	{
		config.readFileContent(configFilePath);
		config.printConfigContent();
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}
}
