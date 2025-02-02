#ifndef UTILS_HPP
# define UTILS_HPP

# include <list>
# include <ctime>
# include <string>
# include <vector>
# include <sstream>
# include <fstream>
# include <cstring>
# include <utility>
# include <iostream>
# include <dirent.h>
# include <algorithm>
# include <sys/stat.h>
# include <sys/types.h>

namespace utils
{
	std::string					formatDate(time_t timestamp);
	time_t						get_timestamp(std::string path);
	long long					strToUlong(std::string s);
	bool						is_status_code(int code);
	bool						is_redirection(size_t code);
	bool						is_informational(size_t code);
	bool						is_successful(size_t code);
	bool						is_server_error(size_t code);
	bool						is_client_error(size_t code);
	bool						is_directory(const std::string& path);
	bool						is_regular_file(const std::string& path);
	bool						is_digits(const std::string& str);
	bool						is_path_exists(const std::string& path);
	bool						is_html_genereted_page(const std::string& path);
	bool						ends_with(const std::string& str, const std::string& suffix);
	bool						starts_with(const std::string& str, const std::string& prefix);
	bool						is_str_in_vector(std::string s, const std::vector<std::string>& content);
	bool						has_write_permission(const std::string& path);
	std::string 				get_interpreter(const std::string& s);
	std::string					get_file_content(const std::string& path);
	std::string					itos(int i);
	std::string					get_reason_phrase(int code);
	std::string					str_tolower(std::string s);
	std::string					str_toupper(std::string s);
	std::string					vector_tostr(const std::vector<std::string>& vec);
	std::string					get_status_message(int code);
	std::string					get_MIME_type(std::string path);
	std::string					get_env_value(char **envp, const std::string &variable);
	std::string					generate_path(const std::string& base_path, const std::string& status_code);
	std::string					substr_after_rdel(const std::string& path, std::string del);
	std::string					substr_before_rdel(const std::string& path, std::string del);
	std::vector<std::string>	ft_split(std::string& s, std::string delimeter);
	int							get_dir_entries(const std::string& dirp, std::vector<std::string>& content);
	bool						is_matching_ext(const std::string& s, const std::vector<std::string>& exts);
	std::string					generate_html_error_page(int status_code);
	std::string					generate_html_directory_listing(const std::string& dir_path);
}
#endif /** UTILS_HPP */