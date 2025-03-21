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
	void			setBody(std::string body);
	void			setStatusCode(int code);
	void			setErrorResponse(int code, std::string path);
	void			setPostResponse(int code, const std::string& filename);
	void			setStaticPageResponse(int code, const std::string& path);
	int				getStatusCode( void );
	std::string		getResponse( void ) const;
	std::string		getHeader(const std::string& key) const;

private:
	std::string							_server_name;
	std::string							_body;
	int									_status_code;
	std::vector<std::pair<std::string, std::string> >	_header_fields;
};
#endif /* RESPONSE_HPP */
