#ifndef REQUEST_HPP
#define REQUEST_HPP

# include <iostream>
# include <string>
# include <vector>
# include <map>

/** TODO: 
 * setter + getter
 * 
 */
class Request
{
public:
	Request();
	~Request();

	bool	parse(const std::string& data);
	void	parseStartLine(const std::string& str);

	bool	isValid(); /* to validate header request */

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
