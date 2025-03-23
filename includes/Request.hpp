#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
# include <string>
# include <vector>
# include <map>
//# include "Connection.hpp"

class Request
{
public:
	Request();
	~Request();

	//Connection *connection;

	bool	parse(const std::string& data, const struct sockaddr_in & clientInfo);
	bool	isValid();
	bool	isBodyExpected();
	const std::string&	getBody() const;
	const std::string&	getFullPath() const;
	const std::string	getHeader(const std::string& key) const;
	void	setFullPath(const std::string& path);
	std::vector<std::pair<std::string, std::string> > getHeaders() const;
	void	printHeaders() const;
	void	setHeader(const std::string& key, const std::string& value);
	void	parseClientInfo(const struct sockaddr_in & clientInfo);
	
	std::string		_upload_path;
	std::string		start_line;
	private:
	bool			_is_valid;
	std::string		_full_path;
	std::string		_body;
	std::vector<std::pair<std::string, std::string> >	_header_fields;
	
	void	parseStartLine(const std::string& str);
	void	parseHeader(const std::string& header_lines);
	void	parseBody(const std::string& body);
	void	parseReqUri(std::string uri);
};

std::ostream &	operator<<( std::ostream & o, Request const & i );

#endif
