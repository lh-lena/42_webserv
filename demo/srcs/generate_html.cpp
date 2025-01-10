#include "../includes/generate_html.hpp"

std::string		generate_html_directory_listing(const std::string& dir_path)
{
	std::stringstream html;

	html << "<!DOCTYPE html>\n"
			"<html lang=\"en\">\n"
			"<head>\n"
			"    <meta charset=\"UTF-8\">\n"
			"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
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

std::string		generate_html_error_page(int status_code)
{
	std::stringstream	html;

	html << "<!DOCTYPE html>\n"
			"<html lang=\"en\">\n"
			"<head>\n"
			"    <meta charset=\"UTF-8\">\n"
			"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
			"    <title>"
			<< status_code
			<< " "
			<< get_reason_phrase(status_code)
			<< "</title>\n"
			"    <style>\n"
			"        body {\n"
			"            display: flex;\n"
			"            justify-content: center;\n"
			"            align-items: center;\n"
			"            height: 100vh;\n"
			"            margin: 0;\n"
			"            font-family: Arial, sans-serif;\n"
			"        }\n"
			"        .error-container {\n"
			"            text-align: center;\n"
			"            background: #fff;\n"
			"            padding: 20px 40px;\n"
			"        }\n"
			"        .error-container h1 {\n"
			"            font-size: 3em;\n"
			"            color: #e74c3c;\n"
			"            margin: 0 0 20px;\n"
			"        }\n"
			"        .error-container p {\n"
			"            font-size: 1.2em;\n"
			"            margin: 0 0 15px;\n"
			"        }\n"
			"    </style>"
			"</head>\n"
			"<body>\n"
			"    <div class=\"error-container\">"
			"        <h1>"
			<< get_reason_phrase(status_code)
			<<"</h1>\n"
			"        <p>"
			<< get_status_message(status_code)
			<< "</p>\n"
			"</body>\n"
			"</html>";

	return html.str();
}
