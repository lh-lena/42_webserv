#include "../includes/utils.hpp"

std::vector<std::string>	ft_split(std::string& s, std::string delimeter)
{
	size_t pos = 0;
	std::vector<std::string> parts;

	if (delimeter.empty())
		return parts;
	while ((pos = s.find(delimeter)) != std::string::npos)
	{
		std::string part = s.substr(0, pos);
		if (part.size() > 0)
		{
			parts.push_back(part);
			s.erase(0, pos + delimeter.length());
		}
	}
	parts.push_back(s);
	return parts;
}

int		strToUint(std::string s)
{
	int	val;
	std::istringstream ss(s);
	ss >> val;
	if(ss.fail() || val < 0)
	{
		return -1;
	}
	return val;
}

std::string		intToStr(int i)
{
	std::stringstream ss;

	ss << i;
	std::string str = ss.str();

	return str;
}

bool		is_digits(const std::string& str)
{
	size_t len = str.size();
	for (size_t i = 0; i < len; i++)
	{
		if (!std::isdigit(str[i]))
			return false;
	}
	return true;
}

bool		is_directory(const std::string& path)
{
	struct stat path_stat;
	if (stat(path.c_str(), &path_stat) != 0)
	{
		// std::cerr << "[ERROR]: Accessing path failed: " << std::strerror(errno) << std::endl;
		return false;
	}
	return S_ISDIR(path_stat.st_mode);
}

bool		is_regular_file(const std::string& path)
{
	struct stat path_stat;
	if (stat(path.c_str(), &path_stat) != 0)
	{
		// std::cerr << "[ERROR]: Accessing path failed: " << std::strerror(errno) << std::endl;
		return false;
	}
	return S_ISREG(path_stat.st_mode);
}

std::string		get_env_value(char **envp, const std::string &variable)
{
	std::string value = "";
	size_t var_len = variable.length();
	for (int i = 0; envp[i] != NULL; i++)
	{
		if (!std::strncmp(envp[i], variable.c_str(), var_len) == 0 && envp[i][var_len] == '=')
		{
			value = envp[i] + var_len + 1;
			break;
		}
	}
	return value;
}

bool		is_path_exists(const std::string& path)
{
	std::ifstream file(path.c_str());
	return file.is_open();
}

std::string generate_path(const std::string& base_path, const std::string& status_code)
{
	std::string p = base_path;
	size_t pos = p.find('x');
	if (pos != std::string::npos)
	{
		p.replace(pos, 1, status_code);
	}

	return p;
}


bool		is_status_code(int code)
{
	return (!reasonPhrase(code).empty());
}

std::string	reasonPhrase(int code)
{
	switch(code)
	{
		/** 1xx (Informational): The request was received, continuing process */
		case 100: return "Continue";
		case 101: return "Switching Protocols";

		/** 2xx (Successful): The request was successfully received, understood, and accepted */
		case 200: return "OK";
		case 201: return "Created";
		case 202: return "Accepted";
		case 203: return "Non-Authoritative Information";
		case 204: return "No Content";
		case 205: return "Reset Content";
		case 206: return "Partial Content";

		/** 3xx (Redirection): Further action needs to be taken in order to complete the request */
		case 301: return "Multiple Choices";
		case 302: return "Moved Permanently";
		case 303: return "Found";
		case 304: return "See Other";
		case 305: return "Not Modified";
		case 306: return "Use Proxy";
		case 307: return "Temporary Redirect";

		/** 4xx (Client Error): The request contains bad syntax or cannot be fulfilled */
		case 400: return "Bad Request";
		case 401: return "Unauthorized";
		case 402: return "Payment Required";
		case 403: return "Forbidden";
		case 404: return "Not Found";
		case 405: return "Method Not Allowed";
		case 406: return "Not Acceptable";
		case 407: return "Proxy Authentication Required";
		case 408: return "Request Timeout";
		case 409: return "Conflict";
		case 410: return "Gone";
		case 411: return "Length Required";
		case 412: return "Precondition Failed";
		case 413: return "Payload Too Large";
		case 414: return "URI Too Long";
		case 415: return "Unsupported Media Type";
		case 416: return "Range Not Satisfiable";
		case 417: return "Expectation Failed";
		case 426: return "Upgrade Required";

		/** 5xx (Server Error): The server failed to fulfill an apparently valid request */
		case 500: return "Internal Server Error";
		case 501: return "Not Implemented";
		case 502: return "Bad Gateway";
		case 503: return "Service Unavailable";
		case 504: return "Gateway Timeout";
		case 505: return "HTTP Version Not Supported";
		case 506: return "Variant Also Negotiates";
		case 507: return "Insufficient Storage";
		case 508: return "Loop Detected";
		case 510: return "Not Extended";
		case 511: return "Network Authentication Required";

		default: return std::string();
	}
}

std::string		MIME_type(std::string path)
{
	size_t pos = path.rfind(".");

	if (pos == std::string::npos)
	{
		return std::string(); 
	}
	std::string res = path.substr(pos);

	if(!std::strcmp(res.c_str(), ".htm"))  return "text/html";
	if(!std::strcmp(res.c_str(), ".html")) return "text/html";
	if(!std::strcmp(res.c_str(), ".php"))  return "text/html";
	if(!std::strcmp(res.c_str(), ".css"))  return "text/css";
	if(!std::strcmp(res.c_str(), ".txt"))  return "text/plain";
	if(!std::strcmp(res.c_str(), ".js"))   return "application/javascript";
	if(!std::strcmp(res.c_str(), ".json")) return "application/json";
	if(!std::strcmp(res.c_str(), ".xml"))  return "application/xml";
	if(!std::strcmp(res.c_str(), ".swf"))  return "application/x-shockwave-flash";
	if(!std::strcmp(res.c_str(), ".flv"))  return "video/x-flv";
	if(!std::strcmp(res.c_str(), ".png"))  return "image/png";
	if(!std::strcmp(res.c_str(), ".jpe"))  return "image/jpeg";
	if(!std::strcmp(res.c_str(), ".jpeg")) return "image/jpeg";
	if(!std::strcmp(res.c_str(), ".jpg"))  return "image/jpeg";
	if(!std::strcmp(res.c_str(), ".gif"))  return "image/gif";
	if(!std::strcmp(res.c_str(), ".bmp"))  return "image/bmp";
	if(!std::strcmp(res.c_str(), ".ico"))  return "image/vnd.microsoft.icon";
	if(!std::strcmp(res.c_str(), ".tiff")) return "image/tiff";
	if(!std::strcmp(res.c_str(), ".tif"))  return "image/tiff";
	if(!std::strcmp(res.c_str(), ".svg"))  return "image/svg+xml";
	if(!std::strcmp(res.c_str(), ".svgz")) return "image/svg+xml";
	return "application/text";
}
