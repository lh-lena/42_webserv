#include "../includes/utils.hpp"

std::string		utils::formatDate(time_t timestamp)
{
	/** RFC7231: IMF-fixdate  = day-name "," SP date1 SP time-of-day SP GMT */

	struct tm datetime;
	char buf[100];

	gmtime_r(&timestamp, &datetime);
	std::strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", &datetime);

	return std::string(buf);
}

std::string		utils::getFormattedDateTime( void )
{
	std::time_t now = std::time(NULL);
	std::tm *localTime = std::localtime(&now);

	char buf[30];

	std::strftime(buf, sizeof(buf), "[%Y/%m/%d %H:%M:%S]", localTime);

	return std::string(buf);
}

std::string		utils::get_value(const std::string& key, const std::map<std::string, std::string>& src)
{
	if (src.empty())
	{
		return "";
	}

	if (src.find(key) == src.end())
	{
		return "";
	}

	std::map<std::string, std::string>::const_iterator	it = src.find(key);
	return it->second;
}

void	utils::parse_header_field(const std::string& header_line, std::vector<std::pair<std::string, std::string> >& headers)
{
	if (header_line.empty())
	{
		return;
	}

	size_t pos = header_line.find(":");
	if (pos == std::string::npos)
	{
		return;
	}

	std::string key = header_line.substr(0, pos);
	if (utils::ends_with(key, " "))
	{
		return;
	}
	std::string value = header_line.substr(pos + 1);
	if (std::strncmp(value.c_str(), " ", 1) == 0)
	{
		value.erase(0, 1);
	}

	headers.push_back(std::make_pair(key, value));
}

std::string		utils::get_value(const std::string& key, const std::vector<std::pair<std::string, std::string> >& headers)
{
	if (key.empty() || headers.empty())
	{
		return "";
	}

	std::vector<std::pair<std::string, std::string> >::const_iterator it = headers.begin();

	for (; it != headers.end(); ++it)
	{
		if (it->first == key)
		{
			return it->second;
		}
	}

	return "";
}

std::vector<std::string>	utils::ft_split(const std::string& s, std::string delimeter)
{
	std::string el = s;
	size_t pos = 0;
	std::vector<std::string> parts;

	if (delimeter.empty() || s.empty())
	{
		return parts;
	}

	while ((pos = el.find(delimeter)) != std::string::npos)
	{
		std::string part = el.substr(0, pos);
		if (part.size() > 0)
		{
			parts.push_back(part);
			el.erase(0, pos + delimeter.length());
		}
	}
	parts.push_back(el);
	return parts;
}

double		utils::stod(const std::string& s)
{
	double	val;
	std::stringstream ss(s);
	ss >> val;

	if(ss.fail() || val < 0)
	{
		return -1;
	}
	return val;
}

int		utils::stoi(const std::string& s)
{
	int	val;
	std::stringstream ss(s);
	ss >> val;

	if(ss.fail())
	{
		return -1;
	}
	return val;
}

unsigned long long			utils::stoull(const std::string& s)
{
	unsigned long long val;

	std::stringstream ss(s);
	ss >> val;

	if(ss.fail())
	{
		return 0;
	}
	return val;
}

std::string		utils::itos(int i)
{
	std::stringstream ss;

	ss << i;

	return ss.str();
}

std::string		utils::to_string(unsigned long long i)
{
	std::stringstream ss;

	ss << i;

	return ss.str();
}

std::string		utils::vector_tostr(const std::vector<std::string>& vec)
{
	std::string s = "";
	std::vector<std::string>::const_iterator cit;
	size_t i = 0;

	if (vec.empty())
	{
		return "";
	}

	for (cit = vec.begin(); cit != vec.end(); ++cit, ++i)
	{
		s.append(*cit);
		if (i < vec.size() - 1)
			s.append(", ");
	}

	return s;
}

bool		utils::is_digits(const std::string& str)
{
	size_t len = str.size();
	for (size_t i = 0; i < len; i++)
	{
		if (!std::isdigit(str[i]))
			return false;
	}
	return true;
}

std::string		utils::load_file_content(const std::string& path)
{
	if (is_html_genereted_page(path))
	{
		return path;
	}

	std::ifstream in(path.c_str());
	std::string content((std::istreambuf_iterator<char>(in)),
	std::istreambuf_iterator<char>());

	return content;
}

bool		utils::is_directory(const std::string& path)
{
	struct stat path_stat;
	if (stat(path.c_str(), &path_stat) != 0)
	{
		// std::cerr << "[ERROR] : Accessing path " << path << " failed (dir test): " << std::strerror(errno) << path << std::endl;
		return false;
	}
	return S_ISDIR(path_stat.st_mode);
}

bool		utils::is_regular_file(const std::string& path)
{
	struct stat path_stat;

	if (path.empty())
	{
		return false;
	}

	if (stat(path.c_str(), &path_stat) != 0)
	{
		std::cerr << "[ERROR] : Accessing path " << path << " failed: " << std::strerror(errno) << std::endl;
		return false;
	}

	// Check for regular file and not a symbolic link
    return S_ISREG(path_stat.st_mode) && !S_ISLNK(path_stat.st_mode);
}

/** If file was not provided, function returns a current time */
time_t		utils::get_timestamp(std::string path)
{
	struct stat result;

	if (!path.empty() && stat(path.c_str(), &result) == 0)
	{
		return time_t(result.st_mtime);
	}

	// Get the timestamp for the current date and time
	time_t currentTime;
    time(&currentTime);
    return currentTime;
}

std::string		utils::get_env_value(char **envp, const std::string &variable)
{
	std::string value = "";
	if (variable.empty())
	{
		return "";
	}

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

/** returns substring after delimetr in a path.
 * If failed empty string
 */
std::string		utils::substr_after_rdel(const std::string& path, const std::string& del)
{
	if (del.empty())
	{
		return std::string();
	}

	size_t pos = 0;
	pos = path.rfind(del);
	if (pos == std::string::npos)
	{
		return std::string();
	}

	if (pos + 1 > path.length())
	{
		return std::string();
	}

	return path.substr(pos + 1);
}

std::string		utils::substr_before_rdel(const std::string& path, const std::string& del)
{
	if (del.empty())
	{
		return std::string();
	}

	size_t pos = 0;
	pos = path.rfind(del);
	if (pos == std::string::npos)
	{
		return std::string();
	}

	return path.substr(0, pos);
}

/** Limit length to 255 characters */
std::string		utils::sanitize_file_name(const std::string& filename)
{
	std::string sanitized;

	std::string::const_iterator it = filename.begin();
	for (; it != filename.end(); ++it)
	{
		char c = *it;
		if (isalnum(c) || c == '.' || c == '-' || c == '_')
		{
			sanitized += c;
		}
	}

	return sanitized.substr(0, 255);
}

bool		utils::is_path_exists(const std::string& path)
{
	std::ifstream file(path.c_str());
	return file.is_open();
}

bool		utils::is_status_code(int code)
{
	return (!get_reason_phrase(code).empty());
}

bool	utils::ends_with(const std::string& str, const std::string& suffix)
{
	return (str.size() >= suffix.size() && str.substr(str.size() - suffix.size()) == suffix);
}

bool	utils::starts_with(const std::string& str, const std::string& prefix)
{
	return (str.size() >= prefix.size() && str.substr(0, prefix.size()) == prefix);
}

bool	utils::is_str_in_vector(const std::string& s, const std::vector<std::string>& content)
{
	std::vector<std::string>::const_iterator it = std::find(content.begin(), content.end(), s);
	if (it != content.end())
	{
		return true;
	}
	return false;
}

bool		utils::is_matching_ext(const std::string& s, const std::vector<std::string>& exts)
{
	if (exts.size() == 0)
	{
		return false;
	}

	for (size_t i = 0; i < exts.size(); i++)
	{
		if (ends_with(s, exts[i]))
		{
			return true;
		}
	}

	return false;
}

int		utils::get_dir_entries(const std::string& dirp, std::vector<std::string>& content)
{
	// unsigned char	isFile =0x8; unsigned char isFolder =0x4;
	DIR				*dir;
	struct dirent	*dir_entry;

	if ((dir = opendir(dirp.c_str())) == NULL)
	{
		std::cerr << "[ERROR] : Opening directory " << dirp << " (" << strerror(errno) << ")" << std::endl;
		return -1;
	}

	while ((dir_entry = readdir(dir)) != NULL)
	{
		content.push_back(dir_entry->d_name);
	}

	closedir(dir);
	return 0;
}

std::string		utils::str_tolower(const std::string& s)
{
	std::string new_str(s);

	for (size_t i = 0; i < s.size(); i++)
	{
		new_str[i] = std::tolower(s[i]);
	}
	return new_str;
}

std::string		utils::str_toupper(const std::string& s)
{
	std::string new_str(s);

	for (size_t i = 0; i < s.size(); i++)
	{
		new_str[i] = std::toupper(s[i]);
	}
	return new_str;
}

bool	utils::is_redirection(size_t code)
{
	return (code >= 300 && code < 400);
}

bool	utils::is_successful(size_t code)
{
	return (code >= 200 && code < 300);
}

bool	utils::is_informational(size_t code)
{
	return (code >= 100 && code < 200);
}

bool	utils::is_client_error(size_t code)
{
	return (code >= 400 && code < 500);
}

bool	utils::is_server_error(size_t code)
{
	return (code >= 500 && code < 600);
}

/** checks if a directory has read, write, and execute permissions for owner, group, and others */
bool	utils::has_all_permissions(const std::string& path)
{
	struct stat path_stat;

	if (stat(path.c_str(), &path_stat) == 0)
	{
		if ((path_stat.st_mode & S_IRWXU) == S_IRWXU &&
			(path_stat.st_mode & S_IRWXG) == S_IRWXG &&
			(path_stat.st_mode & S_IRWXO) == S_IRWXO)
		{
			return true;
		}
	}
	return false;
}

bool	utils::has_user_permissions(const std::string& path)
{
	struct stat path_stat;

	if (stat(path.c_str(), &path_stat) == 0)
	{
		if ((path_stat.st_mode & S_IRWXU) == S_IRWXU)
		{
			return true;
		}
	}
	return false;
}

bool	utils::has_executable_permissions(const std::string& path)
{
	struct stat path_stat;

	if (stat(path.c_str(), &path_stat) == 0)
	{
		if ((path_stat.st_mode & S_IXUSR) ||
			(path_stat.st_mode & S_IXGRP) ||
			(path_stat.st_mode & S_IXOTH))
		{
			return true;
		}
	}

	return false;
}

bool	utils::parse_query(const std::string& uri, std::string& new_uri, std::string& query)
{
	if (uri.empty())
	{
		return false;
	}

	if (uri.rfind('?') != std::string::npos)
	{
		query = utils::substr_after_rdel(uri, "?");
		new_uri = utils::substr_before_rdel(uri, "?");
		return true;
	}

	return false;
}

std::string			utils::get_file_extension(const std::string& path)
{
	if (path.empty())
	{
		return std::string();
	}

	size_t pos = path.find_last_of('.');
	if (pos == std::string::npos)
	{
		return std::string();
	}

	size_t pos2 = path.find_first_of('/', pos);
	if (pos2 != std::string::npos)
	{
		return path.substr(pos, pos2);
	}

	return path.substr(pos);
}

std::string		utils::extract_path_info(const std::string& path)
{
	if (path.empty())
	{
		return "/";
	}

	size_t pos = path.find_last_of('.');
	if (pos == std::string::npos)
	{
		return "/";
	}

	size_t pos2 = path.find_first_of('/', pos);
	if (pos2 == std::string::npos)
	{
		return "/";
	}

	return path.substr(pos2);
}

std::string	utils::extract_script_name(const std::string& path, const std::string& ext)
{
	if (path.empty() || ext.empty())
	{
		return "";
	}

	size_t pos = path.find(ext);
	if (pos == std::string::npos)
	{
		return "";
	}

	size_t pos2 = path.find_last_of('/', pos);
	if (pos2 == std::string::npos)
	{
		return "";
	}

	return path.substr(pos2, pos + ext.length());
}

std::string		utils::get_reason_phrase(int code)
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
		case 300: return "Multiple Choices";
		case 301: return "Moved Permanently";
		case 302: return "Found";
		case 303: return "See Other";
		case 304: return "Not Modified";
		case 305: return "Use Proxy";
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

		default: return std::string();
	}
}

std::string		utils::get_status_message(int code)
{
	switch(code)
	{
		/** 1xx (Informational): The request was received, continuing process */
		case 100: return "Continue with the request. This interim response indicates that everything so far is fine and the client should continue with the request and send the entity body.";
		case 101: return "The server understands and will comply with the client's request to switch protocols.";
		case 102: return "The server has received and is processing the request, but no response is available yet.";

		/** 2xx (Successful): The request was successfully received, understood, and accepted */
		case 200: return "The request has succeeded.";
		case 201: return "The request has been fulfilled and resulted in the creation of a new resource.";
		case 202: return "The request has been accepted for processing, but the processing has not been completed.";
		case 203: return "The server is delivering a response that has been \"borrowed\" from another source.";
		case 204: return "The server successfully processed the request, but is not returning any content.";
		case 205: return "The server successfully processed the request, but is not returning any content. Tells the user agent to reset the document view which caused the request to be sent.";
		case 206: return "The server is delivering only part of the resource due to a range request by the client.";

		/** 3xx (Redirection): Further action needs to be taken in order to complete the request */
		case 300: return "The requested resource has multiple representations, each with its own specific location.";
		case 301: return "The requested resource has been permanently moved to a new URI.";
		case 302: return "The requested resource resides temporarily under a different URI.";
		case 303: return "The response to the request can be found under another URI.";
		case 304: return "The requested resource has not been modified since the last time the client fetched it.";
		case 305: return "The requested resource has not been modified since the last time the client fetched it.";
		case 306: return "Use Proxy";
		case 307: return "The requested resource resides temporarily under a different URI.";

		/** 4xx (Client Error): The request contains bad syntax or cannot be fulfilled */
		case 400: return "The request could not be understood by the server due to malformed syntax.";
		case 401: return "The request requires user authentication.";
		case 402: return "This status code is reserved for future use.";
		case 403: return "The server understood the request but refuses to authorize it.";
		case 404: return "The requested resource could not be found on this server.";
		case 405: return "The method specified in the request is not allowed for the resource.";
		case 406: return "The resource identified by the request is only capable of generating responses that are not acceptable to the client.";
		case 407: return "The client must first authenticate itself with the proxy.";
		case 408: return "The server timed out waiting for the client to send the rest of the request.";
		case 409: return "The request could not be completed due to a conflict with the current state of the resource.";
		case 410: return "The requested resource is no longer available and will not be available again.";
		case 411: return "The server refuses to accept the request without a defined Content-Length.";
		case 412: return "The precondition given in the request header evaluated to false.";
		case 413: return "The server is refusing to process the request because the request entity is larger than the server is willing or able to process.";
		case 414: return "The URI requested by the client is longer than the server is willing to interpret.";
		case 415: return "The server refuses to service the request because the entity of the request is in a format not supported by the server.";
		case 416: return "The client has indicated an unacceptable range in the Range header.";
		case 417: return "The expectation given in the Expect header could not be met by this server.";
		case 426: return "The client should switch to using a different protocol.";

		/** 5xx (Server Error): The server failed to fulfill an apparently valid request */
		case 500: return "The server encountered an unexpected condition that prevented it from fulfilling the request.";
		case 501: return "The server does not support the functionality required to fulfill the request.";
		case 502: return "The server, while acting as a gateway or proxy, received an invalid response from an upstream server.";
		case 503: return "The server is currently unable to handle the request due to a temporary overload or scheduled maintenance.";
		case 504: return "The server, while acting as a gateway or proxy, did not receive a timely response from an upstream server.";
		case 505: return "The server does not support, or refuses to support, the HTTP protocol version that was used in the request.";

		default: return std::string();
	}
}

bool	utils::is_html_genereted_page(const std::string& path)
{
	return (std::strncmp(path.c_str(), "<!DOCTYPE html>", 15) == 0 || std::strncmp(path.c_str(), "<html>", 6) == 0);
}

bool	utils::is_host(const std::string& s)
{
	if (s == "localhost")
	{
		return true;
	}
	std::vector<std::string> els = utils::ft_split(s, ".");
	if (els.size() != 4)
	{
		return false;
	}

	for (size_t i = 0; i < els.size(); ++i)
	{
		int i_val = utils::stoi(els[i]);
		if (!utils::is_digits(els[i]) || (i_val > 255 || i_val < 0))
			return false;
	}
	return true;
}

std::string		utils::get_MIME_type(std::string path)
{
	if (path.empty())
	{
		return std::string();
	}

	size_t pos = path.rfind(".");

	if (is_html_genereted_page(path))
	{
		return "text/html";
	}
	if (pos == std::string::npos)
	{
		return std::string();
	}

	std::string res = path.substr(pos);
	if(std::strcmp(res.c_str(), ".htm") == 0)
		return "text/html";
	if(std::strcmp(res.c_str(), ".html") == 0)
		return "text/html";
	if(std::strcmp(res.c_str(), ".php") == 0)
		return "text/html";
	if(std::strcmp(res.c_str(), ".css") == 0)
		return "text/css";
	if(std::strcmp(res.c_str(), ".txt") == 0)
		return "text/plain";
	if(std::strcmp(res.c_str(), ".js") == 0)
		return "application/javascript";
	if(std::strcmp(res.c_str(), ".json") == 0)
		return "application/json";
	if(std::strcmp(res.c_str(), ".xml") == 0)
		return "application/xml";
	if(std::strcmp(res.c_str(), ".swf") == 0)
		return "application/x-shockwave-flash";
	if(std::strcmp(res.c_str(), ".flv") == 0)
		return "video/x-flv";
	if(std::strcmp(res.c_str(), ".png") == 0)
		return "image/png";
	if(std::strcmp(res.c_str(), ".jpe") == 0)
		return "image/jpeg";
	if(std::strcmp(res.c_str(), ".jpeg") == 0)
		return "image/jpeg";
	if(std::strcmp(res.c_str(), ".jpg") == 0)
		return "image/jpeg";
	if(std::strcmp(res.c_str(), ".gif") == 0)
		return "image/gif";
	if(std::strcmp(res.c_str(), ".bmp") == 0)
		return "image/bmp";
	if(std::strcmp(res.c_str(), ".ico") == 0)
		return "image/vnd.microsoft.icon";
	if(std::strcmp(res.c_str(), ".tiff") == 0)
		return "image/tiff";
	if(std::strcmp(res.c_str(), ".tif") == 0)
		return "image/tiff";
	if(std::strcmp(res.c_str(), ".svg") == 0)
		return "image/svg+xml";
	if(std::strcmp(res.c_str(), ".svgz") == 0)
		return "image/svg+xml";
	return "application/text";
}
