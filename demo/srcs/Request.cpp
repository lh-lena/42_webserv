#include "../includes/Request.hpp"
#include "../includes/Location.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Request::Request() :
	method(std::string()),
	host(std::string()),
	reqBody(std::string()),
	reqURI(std::string()),
	protocol(std::string()),
	query(std::string()),
	charset(std::string()),
	contentType(std::string()),
	fileUpload(std::string()),
	dirUpload(std::string()),
	cgiInterpreter(std::string()),
	contentLength(0)
{}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Request::~Request() {}

/*
** --------------------------------- METHODS ----------------------------------
*/



/*
** --------------------------------- ACCESSOR ---------------------------------
*/

std::ostream &			operator<<( std::ostream & o, Request const & i )
{
	o	<< "method = " << i.method << std::endl
		<< "host = " << i.host << std::endl
		<< "reqURI = " << i.reqURI << std::endl
		<< "protocol = " << i.protocol << std::endl
		<< "contentLength = " << i.contentLength << std::endl;

	return o;
}

bool	Request::isCGIRequest(const Location& loc) const
{

}