# include "../includes/Response.hpp"
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
	std::string body;
	std::string date = utils::formatDate(utils::get_timestamp(""));

	if (path.empty())
	{
		body = utils::generate_html_error_page(code);
		setHeader("Content-Type", utils::get_MIME_type(body));
	}
	else
	{
		body = utils::load_file_content(path);
		date = utils::formatDate(utils::get_timestamp(path));
		setHeader("Content-Type", utils::get_MIME_type(path));
	}

	setBody(path);
	setHeader("Date", date);
	setHeader("Content-Length", utils::itos(body.length()));
}

std::string		Response::getResponse( void ) const
{
	std::ostringstream response;
	std::string			CRLF = "\r\n";

	response << "HTTP/1.1 " << _status_code << " " << utils::get_reason_phrase(_status_code) << CRLF;

	for (std::map<std::string, std::string>::const_iterator it = _headers.begin(); it != _headers.end(); ++it)
	{
		response << it->first << ": " << it->second << CRLF;
	}

	response << CRLF;
	response << _body;

	return response.str();
}
