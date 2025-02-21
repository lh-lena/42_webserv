# include "../includes/Response.hpp"
# include "../includes/utils.hpp"

Response::Response() :_server_name("42_webserv/1.0") {}

Response::~Response() {}

void	Response::setHeader(const std::string& key, const std::string& value)
{
	_header_fields.push_back(std::make_pair(key, value));
}

void	Response::setBody(const std::string& body)
{
	_body = body;
}

void	Response::setStatusCode(int code)
{
	_status_code = code;
}

int		Response::getStatusCode()
{
	return _status_code;
}

std::string Response::getHeader(const std::string& key) const
{
	std::vector<std::pair<std::string, std::string> >::const_iterator it = _header_fields.begin();
	for (; it != _header_fields.end(); ++it)
	{
		if (it->first == key)
		{
			return it->second;
		}
	}
	return "";
}

void	Response::setStaticPageResponse(int code, const std::string& path)
{
	std::string	body = utils::load_file_content(path);
	unsigned long long con_len = body.length();

	/* if (con_len <= 0)
	{
		status_code = NO_CONTENT;
	} */

	setStatusCode(code);
	setBody(body);
	setHeader("Date", utils::formatDate(utils::get_timestamp("")));
	setHeader("Server", _server_name);
	setHeader("Last-Modified", utils::formatDate(utils::get_timestamp(path)));
	setHeader("Content-Type", utils::get_MIME_type(path));
	setHeader("Content-Length", utils::to_string(con_len));
}

void	Response::setErrorResponse(int code, std::string path)
{
	setStatusCode(code);
	std::string body;
	std::string date = utils::formatDate(utils::get_timestamp(""));
	std::string last_modified = utils::formatDate(utils::get_timestamp(""));

	if (path.empty())
	{
		body = utils::generate_html_error_page(code);
		last_modified = utils::formatDate(utils::get_timestamp(path));
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
	setHeader("Server", _server_name);
	setHeader("Last-Modified", last_modified);
	setHeader("Content-Length", utils::itos(body.length()));
}

std::string		Response::getResponse( void ) const
{
	std::ostringstream response;

	response	<< "HTTP/1.1 " 
				<< _status_code
				<< " " 
				<< utils::get_reason_phrase(_status_code)
				<< CRLF;

	std::vector<std::pair<std::string, std::string> >::const_iterator it = _header_fields.begin();
	for (; it != _header_fields.end(); ++it)
	{
		response << it->first << ": " << it->second << CRLF;
	}

	response << CRLF;
	response << _body;

	return response.str();
}
