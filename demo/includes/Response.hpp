#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
# define CRLF "\r\n"

class	Response
{
public:
	Response();
	~Response();

	void			setHeader(const std::string& key, const std::string& value);
	void			setBody(const std::string& body);
	void			setStatusCode(int code);
	void			setErrorResponse(int code, std::string path);
	void			setStaticPageResponse(int code, const std::string& path);
	std::string		getResponse( void ) const;

private:
	std::string							_server_name;
	std::string							_body;
	int									_status_code;
	// std::map<std::string, std::string>	_header_fields;
	std::vector<std::pair<std::string, std::string> >	_header_fields;
};

#endif /* RESPONSE_HPP */