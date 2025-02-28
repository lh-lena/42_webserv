#ifndef UTILS_HPP
# define UTILS_HPP

# include <map>
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

# define GRAY  "\033[90m"
# define BLUE   "\033[94m"
# define MAGENTA "\033[95m"
# define CYAN   "\033[96m"
# define WHITE  "\033[97m"
# define RED "\033[31m"
# define GREEN "\033[32m"
# define YELLOW "\033[33m"
# define RESET "\033[0m"

namespace utils
{
	std::string					formatDate(time_t timestamp);
	time_t						get_timestamp(std::string path);
	double						stod(const std::string& s);
	int							stoi(const std::string& s);
	unsigned long long			stoull(const std::string& s);
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
	bool						is_host(const std::string& s);
	bool						ends_with(const std::string& str, const std::string& suffix);
	bool						starts_with(const std::string& str, const std::string& prefix);
	bool						is_str_in_vector(const std::string& s, const std::vector<std::string>& content);
	bool						has_all_permissions(const std::string& path);
	bool						has_user_permissions(const std::string& path);
	bool						has_executable_permissions(const std::string& path);
	bool						parse_query(const std::string& uri, std::string& new_uri, std::string& query);
	void						parse_header_field(const std::string& header_line, std::vector<std::pair<std::string, std::string> >& headers);
	std::string					getFormattedDateTime( void );
	std::string					get_value(const std::string& key, const std::vector<std::pair<std::string, std::string> >& headers);
	std::string					get_value(const std::string& key, const std::map<std::string, std::string>& src);
	std::string					get_file_extension(const std::string& path);
	std::string					extract_path_info(const std::string& path);
	std::string					extract_script_name(const std::string& path, const std::string& ext);
	std::string					load_file_content(const std::string& path);
	std::string					itos(int i);
	std::string					to_string(unsigned long long i);
	std::string					get_reason_phrase(int code);
	std::string					str_tolower(const std::string& s);
	std::string					str_toupper(const std::string& s);
	std::string					vector_tostr(const std::vector<std::string>& vec);
	std::string					get_status_message(int code);
	std::string					get_MIME_type(std::string path);
	std::string					get_env_value(char **envp, const std::string &variable);
	std::string					substr_after_rdel(const std::string& path, const std::string& del);
	std::string					substr_before_rdel(const std::string& path, const std::string& del);
	std::string					sanitize_file_name(const std::string& filename);
	std::vector<std::string>	ft_split(const std::string& s, std::string delimeter);
	int							get_dir_entries(const std::string& dirp, std::vector<std::string>& content);
	bool						is_matching_ext(const std::string& s, const std::vector<std::string>& exts);
}
#endif /** UTILS_HPP */
