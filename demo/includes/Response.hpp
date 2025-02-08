#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>

class	Response
{
public:
	Response();
	~Response();

	void			setHeader(const std::string& key, const std::string& value);
	void			setBody(const std::string& body);
	void			setStatusCode(int code);
	void			setErrorPage(int code, std::string path);
	std::string		getResponse( void ) const;


	std::string							_body;
	int									_status_code;
	std::map<std::string, std::string>	_headers;
};

std::ostream &			operator<<( std::ostream & o, Response const & i );

#endif /* RESPONSE_HPP */