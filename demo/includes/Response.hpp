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

/** TODO: rm */
	int			status_code;
	long long	content_lenght;
	bool		location_found;
	std::string	reqURI;
	std::string	path;
	std::string	error_path;
	std::string	uploadDir;
	std::string	uploadFile;
	std::string	method;
	std::string	protocol;
	std::string	query;
	std::string	reason_phrase;
	std::string	date;
	std::string	last_mod;
	std::string	content;
	std::string	server;
	std::string	allow;
	std::string	host;
	std::string	content_type;
	std::string	location;
};

std::ostream &			operator<<( std::ostream & o, Response const & i );

#endif /* RESPONSE_HPP */