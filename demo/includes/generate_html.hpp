#ifndef GENERATES_HTML_HPP
# define GENERATES_HTML_HPP

# include <string>
# include <vector>
# include "utils.hpp"

std::string		generate_html_directory_listing(const std::string& dir_path);
std::string		generate_html_error_page(int status_code);

#endif /** GENERATES_HTML_HPP */
