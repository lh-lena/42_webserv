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

bool		is_digits(const std::string& str)
{
	size_t len = str.size();
	for (int i = 0; i < len; i++)
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
		if (std::strncmp(envp[i], variable.c_str(), var_len) == 0 && envp[i][var_len] == '=')
		{
			value = envp[i] + var_len + 1;
			break;
		}
	}
	return value;
}

bool		is_path_exists(const std::string& path)
{
	std::ifstream file(path);
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
	std::string tmp = reasonPhrase(code);
	return (!tmp.empty());
}

std::string	reasonPhrase(int code)
{
	switch(code)
	{
		/** 1xx (Informational): The request was received, continuing process */
		case 100: return "Continue";
		case 101: return "Switching Protocols";

		/** 2xx (Successful): The request was successfully received,
      understood, and accepted */
		case 200: "OK";
		case 201: "Created";
		case 202: "Accepted";
		case 203: "Non-Authoritative Information";
		case 204: "No Content";
		case 205: "Reset Content";
		case 206: "Partial Content";

	    /** 3xx (Redirection): Further action needs to be taken in order to
      complete the request */
		case 301: "Multiple Choices";
		case 302: "Moved Permanently";
		case 303: "Found";
		case 304: "See Other";
		case 305: "Not Modified";
		case 306: "Use Proxy";
		case 307: "Temporary Redirect";

		/** 4xx (Client Error): The request contains bad syntax or cannot be
      fulfilled */
		case 400: "Bad Request";
		case 401: "Unauthorized";
		case 402: "Payment Required";
		case 403: "Forbidden";
		case 404: "Not Found";
		case 405: "Method Not Allowed";
		case 406: "Not Acceptable";
		case 407: "Proxy Authentication Required";
		case 408: "Request Timeout";
		case 409: "Conflict";
		case 410: "Gone";
		case 411: "Length Required";
		case 412: "Precondition Failed";
		case 413: "Payload Too Large";
		case 414: "URI Too Long";
		case 415: "Unsupported Media Type";
		case 416: "Range Not Satisfiable";
		case 417: "Expectation Failed";
		case 426: "Upgrade Required";

		/** 5xx (Server Error): The server failed to fulfill an apparently
      valid request */
		case 500: "Internal Server Error";
		case 501: "Not Implemented";
		case 502: "Bad Gateway";
		case 503: "Service Unavailable";
		case 504: "Gateway Timeout";
		case 505: "HTTP Version Not Supported";
		case 506: return "Variant Also Negotiates";
		case 507: return "Insufficient Storage";
		case 508: return "Loop Detected";
		case 510: return "Not Extended";
		case 511: return "Network Authentication Required";

		default: return std::string();
	}
}
