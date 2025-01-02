#ifndef UTILS_HPP
# define UTILS_HPP

# include <string>
# include <vector>
# include <sstream>
# include <fstream>
# include <cstring>
# include <utility>
# include <iostream>
# include <sys/stat.h>

bool						is_directory(const std::string& path);
bool						is_regular_file(const std::string& path);
bool						is_digits(const std::string& str);
bool						is_path_exists(const std::string& path);
std::string					get_env_value(char **envp, const std::string &variable);
std::vector<std::string>	ft_split(std::string& s, std::string delimeter);
std::string generate_path(const std::string& base_path, const std::string& status_code);

#endif /** UTILS_HPP */