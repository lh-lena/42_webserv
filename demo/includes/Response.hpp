#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <vector>
#include <map>

struct	Response
{
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

	Response()
	{
		content_lenght = 0;
		status_code = 0;
		location_found = false;
		reqURI = std::string();
		path = std::string();
		error_path = std::string();
		uploadDir = std::string();
		uploadFile = std::string();
		method = std::string();
		protocol = std::string();
		query = std::string();
		reason_phrase = std::string();
		date = std::string();
		last_mod = std::string();
		content = std::string();
		server = std::string();
		allow = std::string();
		content_type = std::string();
		location = std::string();
	};
};

std::ostream &			operator<<( std::ostream & o, Response const & i )
{
	o	<< "method = " << i.method << std::endl
		<< "host = " << i.host << std::endl
		<< "reqURI = " << i.reqURI << std::endl
		<< "uploadFile = " << i.uploadFile << std::endl
		<< "error_path = " << i.error_path << std::endl
		<< "uploadDir = " << i.uploadDir << std::endl
		<< "protocol = " << i.protocol << std::endl
		<< "query = " << i.query << std::endl
		<< "protocol = " << i.protocol << std::endl;

	return o;
}

#endif /* RESPONSE_HPP */