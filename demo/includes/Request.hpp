#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
# include <string>
# include <vector>
# include <map>

class Request
{
public:
	Request();
	~Request();

	bool								parse(const std::string& data);

	std::string							getMethod() const;
	std::string							getURI() const;
	std::string							getQueryString() const;
	std::string							getProtocol() const;
	std::string							getHeader(const std::string& key) const;
	std::string							getBody() const;

	bool								isValid(); /* to validate header request ??*/
	bool								isBodyExpected();

	private:
	bool								_is_valid;
	std::string							_method;
	std::string							_reqURI;
	std::string							_protocol;
	std::string							_query;
	std::string							_body;
	std::map<std::string, std::string>	_header_fields;

	void								parseStartLine(const std::string& str);
	void								parseHeader(const std::string& header_lines);
	void								parseBody(const std::string& body);
};

std::ostream &			operator<<( std::ostream & o, Request const & i );

#endif
