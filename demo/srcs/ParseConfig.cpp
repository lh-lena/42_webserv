#include "../includes/ParseConfig.hpp"

/*
** ------------------------------ DECLARATION ---------------------------------
*/

template void	ParseConfig::handleWorkCont<Server>(const std::string&, Server*);
template void	ParseConfig::handleHttpBlock<Server>(const std::string&, Server*);
template void	ParseConfig::handleServerBlock<Server>(const std::string&, Server*);
// template void	ParseConfig::handleErrorLog<Server>(const std::string&, Server*);
// template void	ParseConfig::handleClientBodySize<Server>(const std::string&, Server*);
// template void	ParseConfig::handleListen<Server>(const std::string&, Server*);
// template void	ParseConfig::handleLocationBlock<Server>(const std::string&, Server*);
// template void	ParseConfig::handleServerName<Server>(const std::string&, Server*);
template void	ParseConfig::handleRoot<Location>(const std::string&, Location*);
template void	ParseConfig::handleRoot<Server>(const std::string&, Server*);
// template void	ParseConfig::handleIndex<Location>(const std::string&, Location*);
// template void	ParseConfig::handleIndex<Server>(const std::string&, Server*);
// template void	ParseConfig::handleTimeout<Location>(const std::string&, Location*);
// template void	ParseConfig::handleTimeout<Server>(const std::string&, Server*);
// template void	ParseConfig::handleErrorPage<Location>(const std::string&, Location*);
// template void	ParseConfig::handleErrorPage<Server>(const std::string&, Server*);
// template void	ParseConfig::handleAllowedMethods<Location>(const std::string&, Location*);
// template void	ParseConfig::handleUploadDir<Location>(const std::string&, Location*);
// template void	ParseConfig::handleAutoindex<Location>(const std::string&, Location*);

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

ParseConfig::ParseConfig(std::string file_path, char **envp) : _conf_file_path("serv.conf"), _envp(envp)
{
	if (!file_path.empty())
		_conf_file_path = file_path;
	this->setGlobalDirective("worker_connections", &ParseConfig::handleWorkCont);
	// this->setGlobalDirective("http", &ParseConfig::handleHttpBlock);
	// this->setHttpDirective("server", &ParseConfig::handleServerBlock);
	// this->setHttpDirective("error_log", &ParseConfig::handleErrorLog);
	// this->setHttpDirective("client_max_body_size", &ParseConfig::handleClientBodySize);
	this->setServerDirective("root", &ParseConfig::handleRoot);
	// this->setServerDirective("index", &ParseConfig::handleIndex);
	// this->setServerDirective("listen", &ParseConfig::handleListen);
	// this->setServerDirective("timeout", &ParseConfig::handleTimeout);
	// this->setServerDirective("location", &ParseConfig::handleLocationBlock);
	// this->setServerDirective("error_page", &ParseConfig::handleErrorPage);
	// this->setServerDirective("server_name", &ParseConfig::handleServerName);
	// this->setServerDirective("client_max_body_size", &ParseConfig::handleClientBodySize);
	this->setLocationDirective("root", &ParseConfig::handleRoot);
	// this->setLocationDirective("index", &ParseConfig::handleIndex);
	// this->setLocationDirective("return", &ParseConfig::handleReturn);
	// this->setLocationDirective("timeout", &ParseConfig::handleTimeout);
	// this->setLocationDirective("autoindex", &ParseConfig::handleAutoindex);
	// this->setLocationDirective("error_page", &ParseConfig::handleErrorPage);
	// this->setLocationDirective("allowed_methods", &ParseConfig::handleAllowedMethods);
	// this->setLocationDirective("upload_directory", &ParseConfig::handleUploadDir);
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
	Location location;
	// while (it != it_end)
	// {
		directive = *it;
		std::cout << "[LOG] directive " << directive << "  " <<(_server_directives.find(directive) != _server_directives.end()) << std::endl;

		if (_server_directives.find(directive) != _server_directives.end())
		{
			exceptTocken(&_conf_content, directive);
			it = _conf_content.begin();
			value = *it;
			std::cout << "value" << value << std::endl;
			// while (it != it_end)
			// {
				// value = value.substr(0, value.size()-1);
				// if (value.find(";") != std::string::npos)
				// 	;
			// }
				DirectiveServerHandler serv_handler = _server_directives[directive];
				(this->*serv_handler)(value, (Server*)&server);
				std::cout << "Server: " << server.getRoot() << std::endl;
				DirectiveLocationHandler loc_handler = _location_directives[directive];
				(this->*loc_handler)(value, (Location*)&location);
				std::cout << "Location: " << location.getRoot() << std::endl;

			return ;
		}
		else
		{
			std::cerr << "Unknown directive: " << directive << std::endl;
		}
	// 	return ;
	// }
}

template<typename T> void	ParseConfig::handleWorkCont(const std::string& value, T* instance)
{
	std::cout << "[LOG] processing value: " << value << std::endl;
	std::istringstream iss(value);
	int val = 0;
	iss >> val;
	// if (iss.bad())

	instance->setWorkCont(val);
}

template<typename T> void	ParseConfig::handleHttpBlock(const std::string& value, T* instance)
{
		std::cout << "[LOG] processing value: " << value << std::endl;

}

template<typename T> void	ParseConfig::handleServerBlock(const std::string& value, T* instance)
{
	std::cout << "[LOG] processing value: " << value << std::endl;

}

template<typename T> void	ParseConfig::handleRoot(const std::string& value, T* instance)
{
	std::cout << "[LOG] processing value: " << value << std::endl;
	instance->setRoot(value);
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
		// config.printConfigContent();
		config.parseConfigContent();
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}
}
