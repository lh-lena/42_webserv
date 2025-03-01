#include "../includes/ParseConfig.hpp"
#include "../includes/utils.hpp"

/*
** ------------------------------ DECLARATION ---------------------------------
*/
template void	ParseConfig::handleClientBodySize<Server>(const std::pair<std::string, int>&, Server*);
template void	ParseConfig::handleErrorPage<Server>(const std::pair<std::string, int>&, Server*);
template void	ParseConfig::handleIndex<Server>(const std::pair<std::string, int>&, Server*);
template void	ParseConfig::handleRoot<Server>(const std::pair<std::string, int>&, Server*);
template void	ParseConfig::handleUploadDir<Server>(const std::pair<std::string, int>&, Server*);
template void	ParseConfig::handleCgiInterpreter<Server>(const std::pair<std::string, int>&, Server*);
template void	ParseConfig::handleCGIExtension<Server>(const std::pair<std::string, int>&, Server*);
template void	ParseConfig::handleClientBodySize<Location>(const std::pair<std::string, int>&, Location*);
template void	ParseConfig::handleErrorPage<Location>(const std::pair<std::string, int>&, Location*);
template void	ParseConfig::handleIndex<Location>(const std::pair<std::string, int>&, Location*);
template void	ParseConfig::handleRoot<Location>(const std::pair<std::string, int>&, Location*);
template void	ParseConfig::handleUploadDir<Location>(const std::pair<std::string, int>&, Location*);
template void	ParseConfig::handleCgiInterpreter<Location>(const std::pair<std::string, int>&, Location*);
template void	ParseConfig::handleCGIExtension<Location>(const std::pair<std::string, int>&, Location*);

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

ParseConfig::ParseConfig(std::string file_path, char **envp) : _envp(envp), _conf_file_path("serv.conf")
{
	if (!file_path.empty())
		_conf_file_path = file_path;
	std::cout << "_conf_file_path " << _conf_file_path << "\n";
	block_dir["http"] = true;
	block_dir["server"] = true;
	block_dir["location"] = true;
	method["GET"] = true;
	method["HEAD"] = true;
	method["POST"] = true;
	method["DELETE"] = true;
	map_template_dir["error_page"] = true;
	map_template_dir["return"] = true;
	map_template_dir["cgi_interpreter"] = true;
	this->setGlobalDirective("worker_connections", &ParseConfig::handleWorkCont);
	this->setGlobalDirective("http", &ParseConfig::handleHttpBlock);
	this->setHttpDirective("server", &ParseConfig::handleServerBlock);
	this->setHttpDirective("error_log", &ParseConfig::handleErrorLog);
	this->setHttpDirective("client_max_body_size", &ParseConfig::handleClientBodySize);
	this->setServerDirective("root", &ParseConfig::handleRoot);
	this->setServerDirective("index", &ParseConfig::handleIndex);
	this->setServerDirective("listen", &ParseConfig::handleListen);
	this->setServerDirective("error_page", &ParseConfig::handleErrorPage);
	this->setServerDirective("upload", &ParseConfig::handleUploadDir);
	this->setLocationDirective("location", &ParseConfig::handleLocationBlock);
	this->setServerDirective("location", &ParseConfig::handleServerBlock);
	this->setServerDirective("server_name", &ParseConfig::handleServerName);
	this->setServerDirective("client_max_body_size", &ParseConfig::handleClientBodySize);
	this->setServerDirective("cgi_interpreter", &ParseConfig::handleCgiInterpreter);
	this->setLocationDirective("root", &ParseConfig::handleRoot);
	this->setLocationDirective("path", &ParseConfig::handlePath);
	this->setLocationDirective("index", &ParseConfig::handleIndex);
	this->setLocationDirective("alias", &ParseConfig::handleAlias);
	this->setLocationDirective("return", &ParseConfig::handleRedirect);
	this->setLocationDirective("autoindex", &ParseConfig::handleAutoindex);
	this->setLocationDirective("error_page", &ParseConfig::handleErrorPage);
	this->setLocationDirective("method", &ParseConfig::handleAllowedMethods);
	this->setLocationDirective("upload", &ParseConfig::handleUploadDir);
	this->setLocationDirective("cgi_extension", &ParseConfig::handleCGIExtension);
	this->setLocationDirective("cgi_interpreter", &ParseConfig::handleCgiInterpreter);
	this->setLocationDirective("client_max_body_size", &ParseConfig::handleClientBodySize);
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
	if (utils::is_directory(_conf_file_path))
		throw ParseException(utils::getFormattedDateTime() + " [emerg] : open() " + _conf_file_path + " failed (Is a directory)");

	std::ifstream conf_file(_conf_file_path.c_str());

	if (!conf_file.is_open())
		throw ParseException(utils::getFormattedDateTime() + " [emerg] : open() " + _conf_file_path + " failed (" + std::strerror(errno) + ")");

	std::string line;
	int i = 0;
	while (std::getline(conf_file, line))
	{
		std::istringstream					stream(line);
		std::string							word;
		std::pair<std::string, int>			el;
		std::pair<std::string, int>			el2;

		i += 1;
		while (stream >> word)
		{
			size_t last_char_pos = word.size() - 1;
			char last_char = word[last_char_pos];

			if (word.size() - 1 > 0 && (last_char == ';' || last_char  == '{' || last_char == '}'))
			{
				std::string symbol(1, last_char);
				word.erase(last_char_pos);
				el.first = word;
				el.second = i;
				_conf_content.push_back(el);
				el2.first = symbol;
				el2.second = i;
				_conf_content.push_back(el2);
			}
			else if (word == "#" || word[0] == '#')
				break;
			else
			{
				el.first = word;
				el.second = i;
				_conf_content.push_back(el);
			}
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
	std::pair<std::string, int>	el;
	std::string					directive;
	std::string					value;
	Server						server;

	if (_conf_content.empty())
		throw ParseException(utils::getFormattedDateTime() + " [emerg] : Unexpected end of configuration file " + _conf_file_path);
	while (!_conf_content.empty())
	{
		el = getToken(&_conf_content);
		directive = el.first;
		if (_global_directives.find(directive) == _global_directives.end())
			throw ParseException(utils::getFormattedDateTime() + " [emerg] parseConfigContent Unknown directive: " + directive + " in " + _conf_file_path);
		exceptTocken(&_conf_content, el, 0);
		el = getToken(&_conf_content);
		value = el.first;
		DirectiveServerHandler serv_handler = _global_directives[directive];
		(this->*serv_handler)(el, (Server*)&server);
		if (block_dir.find(directive) == block_dir.end())
		{
			exceptTocken(&_conf_content, el, 0);
			el.first = ";";
			exceptTocken(&_conf_content, el, 1);
		}
	}
	if (!_conf_content.empty())
		throw ParseException(utils::getFormattedDateTime() + " [emerg] : Unexpected data in configuration file " + _conf_file_path);
	/** Create a default server */
	if (_serverControler.getServBlockNbr() <= 0)
	{
		server.defaultServer();
		_serverControler.setServer(server);
	}
	// std::cout << _serverControler << std::endl;
}

void		ParseConfig::handleHttpBlock(const std::pair<std::string, int>& value, Server* instance)
{
	std::pair<std::string, int>	el;
	std::string 				directive;
	std::string 				val;

	el = getToken(&_conf_content);
	el.first = "{";
	exceptTocken(&_conf_content, el, 1);
	while (!_conf_content.empty())
	{
		el = getToken(&_conf_content);
		directive = el.first;
		if (directive == "}")
			break;
		if (_http_directives.find(directive) == _http_directives.end())
			throw ParseException(utils::getFormattedDateTime() + " [emerg] unknown directive " + directive + " in " + _conf_file_path + ":" + utils::itos(value.second));
		exceptTocken(&_conf_content, el, 0);
		el = getToken(&_conf_content);
		val = el.first;
		if (block_dir.find(directive) != block_dir.end())
		{
			Server server(*instance);
			DirectiveServerHandler serv_handler = _http_directives[directive];
			(this->*serv_handler)(el, &server);
			_serverControler.setServer(server);
			continue;
		}
		while (val != ";" && (_http_directives.find(val) == _http_directives.end()))
		{
			DirectiveServerHandler serv_handler = _http_directives[directive];
			(this->*serv_handler)(el, instance);
			exceptTocken(&_conf_content, el, 0);
			el = getToken(&_conf_content);
			val = el.first;
		}
		el.first = ";";
		el.second -= 1;
		exceptTocken(&_conf_content, el, 1);
	}
	el = getToken(&_conf_content);
	el.first = "}";
	el.second -= 1;
	exceptTocken(&_conf_content, el, 1);
}

void		ParseConfig::handleServerBlock(const std::pair<std::string, int>& value, Server* instance)
{
	std::pair<std::string, int>	el;
	std::string					directive;
	std::string					val;
	std::string					map_val;

	el = getToken(&_conf_content);
	el.first = "{";
	exceptTocken(&_conf_content, el, 2);
	while (!_conf_content.empty())
	{
		el = getToken(&_conf_content);
		directive = el.first;
		if (directive == "}")
			break;
		if (_server_directives.find(directive) == _server_directives.end())
			throw ParseException(utils::getFormattedDateTime() + " [emerg] unknown directive " + directive + " in " + _conf_file_path + ":" + utils::itos(value.second));
		exceptTocken(&_conf_content, el, 0);
		el = getToken(&_conf_content);
		val = el.first;
		if (block_dir.find(directive) != block_dir.end())
		{
			Location location;
			DirectiveLocationHandler loc_handler = _location_directives[directive];
			(this->*loc_handler)(el, &location);
			if (location.getAllowedMethods().empty())
			{
				location.setAllowedMethod("GET");
			}
			instance->setLocation(location);
			continue;
		}
		if (map_template_dir.find(directive) != map_template_dir.end())
		{
			std::pair<std::string, int> tmp;
			tmp.first = val;
			while (_server_directives.find(tmp.first) == _server_directives.end())
			{
				exceptTocken(&_conf_content, el, 0);
				el = getToken(&_conf_content);
				tmp = el;
				if (tmp.first == ";")
					break ;
				val.append(" " + tmp.first);
			}
			tmp.first = val;
			tmp.second -= 1;
			DirectiveServerHandler serv_handler = _server_directives[directive];
			(this->*serv_handler)(tmp, instance);
		}
		val = el.first;
		while (val != ";" && (_server_directives.find(val) == _server_directives.end()))
		{
			DirectiveServerHandler serv_handler = _server_directives[directive];
			(this->*serv_handler)(el, instance);
			exceptTocken(&_conf_content, el, 0);
			el = getToken(&_conf_content);
			val = el.first;
		}
		el.first = ";";
		el.second -= 1;
		exceptTocken(&_conf_content, el, 1);
	}
	el = getToken(&_conf_content);
	el.first = "}";
	el.second -= 1;
	exceptTocken(&_conf_content, el, 1);
}

void		ParseConfig::handleLocationBlock(const std::pair<std::string, int>& value, Location* instance)
{
	std::pair<std::string, int>	el;
	std::string					map_el;
	std::string					directive;
	std::string					val;

	DirectiveLocationHandler loc_handler = _location_directives["path"];
	(this->*loc_handler)(value, instance);
	el = getToken(&_conf_content);
	exceptTocken(&_conf_content, el, 0);
	el = getToken(&_conf_content);
	el.first = "{";
	exceptTocken(&_conf_content, el, 2);
	while (!_conf_content.empty())
	{
		el = getToken(&_conf_content);
		directive = el.first;
		if (directive == "}")
			break;
		if (_location_directives.find(directive) == _location_directives.end())
			throw ParseException(utils::getFormattedDateTime() + " [emerg] unknown directive " + directive + " in " + _conf_file_path + ":" + utils::itos(value.second));
		exceptTocken(&_conf_content, el, 0);
		el = getToken(&_conf_content);
		val = el.first;
		if (map_template_dir.find(directive) != map_template_dir.end())
		{
			std::pair<std::string, int> tmp;
			tmp.first = val;
			while ( _location_directives.find(tmp.first) == _location_directives.end())
			{
				exceptTocken(&_conf_content, el, 0);
				el = getToken(&_conf_content);
				tmp = el;
				if (tmp.first == ";")
					break ;
				val.append(" " + tmp.first);
			}
			tmp.first = val;
			tmp.second -= 1;
			DirectiveLocationHandler loc_handler = _location_directives[directive];
			(this->*loc_handler)(tmp, instance);
		}
		val = el.first;
		while (val != ";" && (_location_directives.find(val) == _location_directives.end()))
		{
			DirectiveLocationHandler loc_handler = _location_directives[directive];
			(this->*loc_handler)(el, instance);
			exceptTocken(&_conf_content, el, 0);
			el = getToken(&_conf_content);
			val = el.first;
		}
		el = getToken(&_conf_content);
		el.first = ";";
		el.second -= 1;
		exceptTocken(&_conf_content, el, 1);
	}
	el = getToken(&_conf_content);
	el.first = "}";
	el.second -= 1;
	exceptTocken(&_conf_content, el, 1);
}

void		ParseConfig::handleWorkCont(const std::pair<std::string, int>& value, Server* instance)
{
	int val = 0;

	std::string s = value.first;

	if (!utils::is_digits(s))
		throw ParseConfig::ParseException(utils::getFormattedDateTime() + " [emerg] : directive \"worker_connections\" required only digits in " + _conf_file_path + ":" + utils::itos(value.second));
	val = utils::stoi(s);
	if (val <= 0)
		throw ParseConfig::ParseException(utils::getFormattedDateTime() + " [emerg] : directive \"worker_connections\" required a positive number in " + _conf_file_path + ":" + utils::itos(value.second));

	instance->setWorkCont(val);
}

template<typename T> void	ParseConfig::handleRoot(const std::pair<std::string, int>& value, T* instance)
{
	instance->setRoot(value.first);
}

template<typename T>void	ParseConfig::handleUploadDir(const std::pair<std::string, int>& value, T* instance)
{
	instance->setUploadDir(value.first);
}

void		ParseConfig::handlePath(const std::pair<std::string, int>& value, Location* instance)
{
	instance->setPath(value.first);
}

void		ParseConfig::handleErrorLog(const std::pair<std::string, int>& value, Server* instance)
{
	instance->setErrorLog(value.first);
}

void		ParseConfig::handleAlias(const std::pair<std::string, int>& value, Location* instance)
{
	instance->setAlias(value.first);
}

template<typename T> void	ParseConfig::handleClientBodySize(const std::pair<std::string, int>& value, T* instance)
{
	int val = 0;
	size_t res;
	std::string s = value.first;

	std::string unit = s.substr(s.length() - 1);
	std::string nbr = s.substr(0, s.length() - 1);
	if (!utils::is_digits(nbr))
		throw ParseException(utils::getFormattedDateTime() + " [emerg] : directive \"client_max_body_size\" required only digits in " + _conf_file_path + ":" + utils::itos(value.second));
	val = utils::stoi(nbr);
	if (val < 0)
		throw ParseException(utils::getFormattedDateTime() + " [emerg] : directive \"client_max_body_size\" required a positive number in " + _conf_file_path + ":" + utils::itos(value.second));
	if (val > 100)
		throw ParseException(utils::getFormattedDateTime() + " [emerg] : directive \"client_max_body_size\" limited size up to 100 Megabytes in " + _conf_file_path + ":" + utils::itos(value.second));
	if (unit != "M")
		throw ParseException(utils::getFormattedDateTime() + " [emerg] : directive \"client_max_body_size\" required a number's unit in Megabytes in " + _conf_file_path + ":" + utils::itos(value.second));
	
	res = val * 1024 * 1024;
	instance->setClientMaxBody(res);
}

template<typename T> void	ParseConfig::handleCgiInterpreter(const std::pair<std::string, int>& value, T* instance)
{
	std::string					s = value.first;
	std::vector<std::string>	vals;

	vals = utils::ft_split(s, " ");
	int size = vals.size();

	if (size != 2)
		throw ParseException(utils::getFormattedDateTime() + " [emerg] : directive \"_cgi_interpreter\" required a file extention and an executable in " + _conf_file_path + ":" + utils::itos(value.second));
	if (!(utils::starts_with(vals[0], ".")))
		throw ParseException(utils::getFormattedDateTime() + " [emerg] : directive \"_cgi_interpreter\" required a file extention " + _conf_file_path + ":" + utils::itos(value.second));
	std::cerr << YELLOW << "setCgiInterpreter " << vals[0] << " " << vals[1] << RESET << std::endl;
	instance->setCgiInterpreter(vals[0], vals[1]);
}



template<typename T> void	ParseConfig::handleIndex(const std::pair<std::string, int>& value, T* instance)
{
	instance->setIndex(value.first);
}

void		ParseConfig::handleListen(const std::pair<std::string, int>& value, Server* instance)
{
	std::string s = value.first;
	size_t pos = s.find(":");
	int	port_val;

	if (pos == std::string::npos)
	{
		if (utils::is_host(s))
		{
			instance->setPort(8000);
			instance->setHost(s);
			return;
		}
		port_val = utils::stoi(s);
		if(port_val <= 0)
		{
			throw  ParseException(utils::getFormattedDateTime() + " [emerg] : \"port\" required positive numbers only in " + _conf_file_path + ":" + utils::itos(value.second));
		}
		if (instance->getPort() != 0)
		{
			throw  ParseException(utils::getFormattedDateTime() + " [emerg] : a duplicate \"listen\" " + utils::itos(port_val) + "in " + _conf_file_path + ":" + utils::itos(value.second));
		}
		instance->setPort(port_val);
		instance->setHost("127.0.0.1");
		return;
	}
	std::string host = s.substr(0, pos);
	if	(host[0] == '$')
		host = processEnvVar(host);
	std::string port = s.substr(pos + 1);
	if (port[0] == '$')
	port = processEnvVar(port);
	if (host.empty() || port.empty())
		throw ParseException(utils::getFormattedDateTime() + " [emerg] \"" + value.first + "\" invalid input in " + _conf_file_path + ":" + utils::itos(value.second));
	if (!utils::is_host(host))
	{
		throw  ParseException(utils::getFormattedDateTime() + " [emerg] : directive \"listen\" containes an invalid host " + host + " in " + _conf_file_path + ":" + utils::itos(value.second));
	}
	port_val = utils::stoi(port);
	if(port_val <= 0)
	{
		throw  ParseException(utils::getFormattedDateTime() + " [emerg] : \"port\" required positive numbers only in " + _conf_file_path + ":" + utils::itos(value.second));
	}
	if (!instance->getHost().empty() || instance->getPort() != 0)
	{
		throw  ParseException(utils::getFormattedDateTime() + " [emerg] : a duplicate \"listen\" " + host + ":" + utils::itos(port_val) + "in " + _conf_file_path + ":" + utils::itos(value.second));
	}
	instance->setHost(host);
	instance->setPort(port_val);
}

void		ParseConfig::handleServerName(const std::pair<std::string, int>& value, Server* instance)
{
	instance->setServerName(value.first);
}

void		ParseConfig::handleAutoindex(const std::pair<std::string, int>& value, Location* instance)
{
	std::string s = value.first;

	if (s == "on")
		instance->setAutoindex(true);
	else if (s == "off")
		instance->setAutoindex(false);
	else
		throw ParseException(utils::getFormattedDateTime() + " [emerg] : directive \"autoindex\" misses 'on' or 'off' statement in " + _conf_file_path + ":" + utils::itos(value.second));
}

void		ParseConfig::handleAllowedMethods(const std::pair<std::string, int>& value, Location* instance)
{
	std::string s = value.first;

	if (this->method.find(s) != this->method.end())
	{
		instance->setAllowedMethod(s);
	}
	else
	{
		throw ParseException(utils::getFormattedDateTime() + " [emerg] : method \"" + s + "\" not allowed in " + _conf_file_path + ":" + utils::itos(value.second));
	}
}

void	ParseConfig::handleRedirect(const std::pair<std::string, int>& value, Location* instance)
{
	std::string 				s = value.first;
	std::vector<std::string>	vals = utils::ft_split(s, " ");
	std::string					path = "";
	int							code = 0;

	if (vals.empty())
	{
		throw ParseException(utils::getFormattedDateTime() + " [emerg] : invalid number of arguments in \"return\" directive in " + _conf_file_path + ":" + utils::itos(value.second));
	}

	if (vals.size() == 1 && 
		(utils::starts_with(vals[0], "http://") || 
		utils::starts_with(vals[0], "https://")))
	{
		path = vals[0];
		instance->setRedirect(302, path);
		return;
	}

	code = utils::stoi(vals[0]);
	if (code <= 0)
	{
		throw ParseException(utils::getFormattedDateTime() + " [emerg] : invalid return code \"" + vals[0] + "\" in " + _conf_file_path + ":" + utils::itos(value.second));
	}
	if (!(utils::is_redirection(code) || utils::is_server_error(code) || utils::is_client_error(code)))
	{
		throw ParseException(utils::getFormattedDateTime() + " [emerg] : value \"" + vals[0] + "\" must be between 300 and 599 in " + _conf_file_path + ":" + utils::itos(value.second));
	}
	if (vals.size() == 2)
		path = vals[1];
	instance->setRedirect(code, path);
}

template<typename T> void	ParseConfig::handleErrorPage(const std::pair<std::string, int>& value, T* instance)
{
	std::string					s = value.first;
	std::vector<std::string>	vals;
	std::string					path = std::string();
	int							tmp;
	int 						errorCode;

	vals = utils::ft_split(s, " ");
	int size = vals.size();

	if (vals.empty())
	{
		throw ParseException(utils::getFormattedDateTime() + " [emerg] : invalid number of arguments in \"error_page\" directive in " + _conf_file_path + ":" + utils::itos(value.second));
	}

	for (int i = 0; i < size; i++)
	{
		tmp = utils::stoi(vals[size - 1]);
		if (!utils::is_digits(vals[size - 1]) || !utils::is_status_code(tmp))
			path = vals[size - 1];
		if (i == (size - 1) && utils::stoi(vals[size - 1]) == -1)
			break;
		errorCode = utils::stoi(vals[i]);
		if (errorCode <= 0 || !utils::is_status_code(errorCode))
		{
			throw ParseException(utils::getFormattedDateTime() + " [emerg] : invalid status code in \"error_page\" directive in " + _conf_file_path + ":" + utils::itos(value.second));
		}
		if (size == 1)
		{
			instance->addErrorPage(errorCode, std::string());
			break;
		}
		size_t pos = path.find('*');
		if (pos != std::string::npos)
		{
			int divisor = 1;
			while (pos != std::string::npos && divisor > 0)
			{
				size_t pos_2 = path.rfind('.');
				for (size_t i = 0; i < pos_2 - pos - 1; ++i)
				{
					divisor *= 10;
				}
				int digit = (errorCode / divisor) % 10;
				path.replace(pos, 1, utils::itos(digit));
				pos = path.find('*', pos + 1);
				divisor /= 10;
			}
		}
		instance->addErrorPage(errorCode, path);
	}
}

template<typename T> void	ParseConfig::handleCGIExtension(const std::pair<std::string, int>& value, T* instance)
{
	instance->setCGIExtension(value.first);
}

/**
 * The function `exceptTocken` checks if the front element of a list matches a given token and prints an error message accordingly.
 */
int		ParseConfig::exceptTocken(std::list<std::pair<std::string, int> > *src, std::pair<std::string, int> tocken, int expected)
{
	if (src->empty())
		throw ParseException(utils::getFormattedDateTime() + " [emerg] : Unexpected end of configuration file " + _conf_file_path);

	std::pair<std::string, int> front = src->front();
	if (front.first != tocken.first)
	{
		if (expected == 2)
			throw ParseException(utils::getFormattedDateTime() + " [emerg] : directive has no opening \"" + tocken.first + "\" in " + _conf_file_path + ":" + utils::itos(tocken.second));
		else if (expected)
			throw ParseException(utils::getFormattedDateTime() + " [emerg] : is not terminated by \"" + tocken.first + "\" in " + _conf_file_path + ":" + utils::itos(tocken.second));
		else
			throw ParseException(utils::getFormattedDateTime() + " [emerg] : unexpected \"" + tocken.first + "\" in " + _conf_file_path + ":" + utils::itos(tocken.second));
	}

	src->pop_front();
	return 1;
}

/**
 * The function `processEnvVar` parses a string input to extract and return the value of an environment
 * variable specified within `${}`.
 */
std::string ParseConfig::processEnvVar(const std::string &input)
{
	size_t start = input.find("${");
	size_t end = input.find("}");
	std::string	res;

	if (start != std::string::npos && end != std::string::npos && (start + 2) != end)
	{
		std::string var = input.substr(start + 2, end - start - 2);
		res = utils::get_env_value(_envp, var);
		return res;
	}
	else
		return res;
}

std::pair<std::string, int> ParseConfig::getToken(std::list<std::pair<std::string, int> > *src)
{
	if (src->empty())
		throw ParseConfig::ParseException(utils::getFormattedDateTime() + " [emerg] : unexpected end of configuration content in " + _conf_file_path);
	std::pair<std::string, int> token = src->front();
	return token;
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

ServerControler & ParseConfig::getServControler()
{
	return _serverControler;
}



/* ************************************************************************** */

