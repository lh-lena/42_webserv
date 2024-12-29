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