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

	bool	parse(const std::string& data);
	bool	isValid();
	bool	isBodyExpected();
	const std::string&	getBody() const;
	const std::string&	getFullPath() const;
	const std::string	getHeader(const std::string& key) const;
	void	setFullPath(const std::string& path);
	std::vector<std::pair<std::string, std::string> > getHeaders() const;

private:
	bool			_is_valid;
	std::string		_full_path;
	std::string		_upload_path;
	std::string		_body;
	std::vector<std::pair<std::string, std::string> >	_header_fields;

	void	parseStartLine(const std::string& str);
	void	parseHeader(const std::string& header_lines);
	void	parseBody(const std::string& body);
	void	setHeader(const std::string& key, const std::string& value);
};

std::ostream &	operator<<( std::ostream & o, Request const & i );

#endif
