# include "../includes/Response.hpp"

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
