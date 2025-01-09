#include "../includes/generate_html.hpp"

std::string		generate_html_directory_listing(const std::string& dir_path)
{
	std::stringstream html;

	html << "<!DOCTYPE html>\n"
		"<html>\n"
		"<head>\n"
		"	<title>Directory Listing</title>\n"
		"</head>\n"
		"<body>\n"
		"	<h1>Directory Listing for: " << dir_path << "</h1>\n"
		"	<ul>\n";

	std::vector<std::string> dir_content;
	
	if (get_dir_entries(dir_path, dir_content))
	{
		for (size_t i = 0; i < dir_content.size(); i++)
		{
			std::string	d_name = dir_content[i];
			if (d_name != "." && d_name != "..")
			{
				html << "		<li><a href=\"" << d_name << "\">" << d_name << "</a></li>\n";
			}
		}
	}
	else
	{
		html << "		<p>Error: Could not open directory.</p>\n";
	}

	html << "	</ul>\n"
			"</body\n>"
			"</html>";

	return html.str();
}
