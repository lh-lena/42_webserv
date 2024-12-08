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
	HttpServer httpServer;
	_servers = httpServer;
	block_dir["http"] = true;
	block_dir["server"] = true;
	block_dir["location"] = false;
	this->setGlobalDirective("worker_connections", &ParseConfig::handleWorkCont);
	this->setGlobalDirective("http", &ParseConfig::handleHttpBlock);
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

void ParseConfig::parseConfigContent( void )
{
	std::string directive;
	std::string value;
	Server server;
	try
	{
		while (!_conf_content.empty())
		{
			directive = getToken(&_conf_content);
			std::cout << "[LOG] parseConfigContent directive: " << directive << std::endl;
			if (_global_directives.find(directive) == _global_directives.end())
			{
				std::cerr << "[ERROR] parseConfigContent Unknown directive: " << directive << std::endl;
				return ;
			}
			exceptTocken(&_conf_content, directive);
			value = getToken(&_conf_content);
			std::cout << "[LOG] parseConfigContent value: " << value << std::endl;
			DirectiveServerHandler serv_handler = _global_directives[directive];
			(this->*serv_handler)(value, (Server*)&server);
			if (block_dir.find(directive) == block_dir.end())
			{
				exceptTocken(&_conf_content, value);
				exceptTocken(&_conf_content, ";");
			}
		}
		_servers.setServer(server);
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
}

std::string ParseConfig::getToken(std::list<std::string> *src)
{
	if (src->empty())
		throw ParseConfig::ParseException("[emrge] : Unexpected end of configuration content in " + _conf_file_path);
	std::string token = src->front();
	return token;
}

template<typename T> void	ParseConfig::handleWorkCont(const std::string& value, T* instance)
{
	std::cout << "[LOG] handleWorkCont processing value: " << value << std::endl;
	std::istringstream iss(value);
	int val = 0;
	iss >> val;
	// if (iss.bad()) /*TODO: -error checking */

	instance->setWorkCont(val);
}

/* TODO:
- change [T* instance] from [Server] to [HttpServer]
- replace _server_directives to _http_directives
- to handle global data to classes
*/

template<typename T> void	ParseConfig::handleHttpBlock(const std::string& value, T* instance)
{
	std::cout << "[LOG] handleHttpBlock: processing value: " << value << std::endl;
	Server server;
	if (_servers.getServer().size() != 0)
		server = _servers.getServer().front();
	std::string directive;
	std::string val;
	exceptTocken(&_conf_content, "{");
	try
	{
		while (!_conf_content.empty())
		{
			directive = getToken(&_conf_content);
			std::cout << "[handleHttpBlock] directive : " << directive << std::endl;
			if (directive == "}")
				break;
			if (_server_directives.find(directive) == _server_directives.end())
				throw ParseException("[emerg] unknown directive " + directive + " in " + _conf_file_path);
			exceptTocken(&_conf_content, directive);
			val = getToken(&_conf_content);
			std::cout << "[handleHttpBlock] processing value : " << val << std::endl;
			while (val != ";")
			{
				DirectiveServerHandler serv_handler = _server_directives[directive];
				(this->*serv_handler)(val, (Server*)&server);
				exceptTocken(&_conf_content, val);
				val = getToken(&_conf_content);
			}
			exceptTocken(&_conf_content, val);
		}
		_servers.setServer(server);
		exceptTocken(&_conf_content, "}");
	}
	catch(const std::exception& e)
	{
		throw ParseException(e.what());
	}
	
}

template<typename T> void	ParseConfig::handleServerBlock(const std::string& value, T* instance)
{
	std::cout << "[handleServerBlock] processing value: " << value << std::endl;

}

template<typename T> void	ParseConfig::handleRoot(const std::string& value, T* instance)
{
	std::cout << "[handleRoot] processing value: " << value << std::endl;
	instance->setRoot(value);
}

int	ParseConfig::exceptTocken(std::list<std::string> *src, std::string tocken)
{
	if (!src->empty() && src->front() == tocken)
	{
		std::cout << "[exceptTocken] : " << (*src).front() << std::endl;
		src->pop_front();
		return 1;
	}
	else
		std::cout << "[emerg] : unexpected " << tocken << "\" in " << _conf_file_path << std::endl;
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
		config.parseConfigContent();
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}
}
