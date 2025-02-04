#ifndef REQUEST_HPP
#define REQUEST_HPP

# include <iostream>
# include <string>
# include <vector>
# include <map>

class Request
{
public:
	Request();
	~Request();

	bool		isCGIRequest(const Location& loc) const;

	std::string	method;
	std::string	host;
	std::string	reqBody;
	std::string	reqURI; /** the path must be extracted from the request */
	std::string	protocol;
	std::string	query;
	std::string	charset;
	std::string	contentType;
	std::string	fileUpload;
	std::string	dirUpload;
	std::string	cgiInterpreter;
	long long	contentLength;
};

std::ostream &			operator<<( std::ostream & o, Request const & i );

#endif
