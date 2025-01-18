#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <vector>
#include <map>

struct	Response
{
	int			status_code;
	uint		content_lenght;
	bool		location_found;
	std::string	path;
	std::string	method;
	std::string	protocol;
	std::string	reason_phrase;
	std::string	date;
	std::string	last_mod;
	std::string	content;
	std::string	server;
	std::string	allow;
	std::string	content_encoding; // ?
	std::string	content_type;
	std::string	src_location;
};

#endif /* RESPONSE_HPP */