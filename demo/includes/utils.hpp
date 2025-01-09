#ifndef UTILS_HPP
# define UTILS_HPP

# include <list>
# include <string>
# include <vector>
# include <sstream>
# include <fstream>
# include <cstring>
# include <utility>
# include <iostream>
# include <dirent.h>
# include <sys/stat.h>
# include <sys/types.h>

# define FOUND 1
# define NOT_FOUND 0

int							strToUint(std::string s);
bool						is_status_code(int code);
bool						is_directory(const std::string& path);
bool						is_regular_file(const std::string& path);
bool						is_digits(const std::string& str);
bool						is_path_exists(const std::string& path);
bool						ends_with(const std::string& str, const std::string& suffix);
std::string					intToStr(int i);
std::string					get_reason_phrase(int code);
std::string					get_status_message(int code);
std::string					get_env_value(char **envp, const std::string &variable);
std::string					generate_path(const std::string& base_path, const std::string& status_code);
std::vector<std::string>	ft_split(std::string& s, std::string delimeter);
int							get_dir_entries(const std::string& dirp, std::vector<std::string>& content);

#endif /** UTILS_HPP */