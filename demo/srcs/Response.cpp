# include "../includes/Response2.hpp"
# include "../includes/utils.hpp"

Response::Response() {}

Response::~Response() {}

void	Response::setHeader(const std::string& key, const std::string& value)
{
	_headers[key] = value;
}

void	Response::setBody(const std::string& body)
{
	_body = body;
}

void	Response::setStatusCode(int code)
{
	_status_code = code;
}

void	Response::setErrorPage(int code, std::string path)
{
	setStatusCode(code);
	// if (path.empty()) /** TODO: to implement in a invocked function*/
	// {
	// 	path = utils::generate_html_error_page(code);
	// 	setBody(path);
	// 	return;
	// }

	setBody(utils::load_file_content(path));
}

std::string		Response::getResponse( void ) const
{
	std::ostringstream response;
	std::string			CRLF = "\r\n";

	response << "HTTP/1.1 " << _status_code << " " << utils::get_status_message(_status_code) << CRLF;

	for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
	{
        response << it->first << ": " << it->second << CRLF;
    }

	response << CRLF;
	response << _body;

	return response.str();
}
