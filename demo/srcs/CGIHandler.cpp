
#include "../includes/CGIHandler.hpp"
#include "../includes/Server.hpp"

// CGIHandler::CGIHandler(const Server& server, const Location& loc) : 
// {}


// void CGIHandler::setEnvironment(const Request& request)
// {
// 	env["REQUEST_METHOD"] = request.method;
// 	env["QUERY_STRING"] = request.query;
// 	env["GATEWAY_INTERFACE"] = "CGI/1.1";
// 	env["SERVER_PROTOCOL"] = request.protocol;
// }

/** NOTES:
 * - parse cgi request
 * - path also a cgi path to the executable file without query
 * - 
 */

int		Server::handleCGI(Response& response, Location& loc)
{
	std::cout << "response.status_code " << response.status_code << std::endl;

	response.uploadDir = substr_before_rdel(response.path, "/");
	std::cout << "response.uploadDir1 " << response.uploadDir << std::endl;
	response.uploadDir = Server::searchingUploadDir(response.reqURI, &loc);

	std::cout << "response.uploadDir2 " << response.uploadDir << std::endl;

	std::cout << "response.path1 " << response.path << std::endl;
	if (response.path.find("?") != std::string::npos) /* check queries */
	{
		response.query = substr_after_rdel(response.path, "?");
		response.path = substr_before_rdel(response.path, "?");
	}
	std::cout << "is_regular_file(response.path) " << is_regular_file(response.path) << std::endl;
	if (is_regular_file(response.path))
	{
		std::cout << "is_matching_ext(response.path, loc->getCgiExtension()) " << is_matching_ext(response.path, loc.getCgiExtension()) << std::endl;
		if (!response.location_found || !is_matching_ext(response.path, loc.getCgiExtension()))
		{
			return 1;
		}
		response.uploadFile = substr_after_rdel(response.path, "/");
		std::cout << "response.uploadFile1 " << response.uploadFile << std::endl;
		// setCGIResponse(response, OK);
		setGetResponse(response, OK);
		return 0;
	}
	else if (!is_directory(response.path))
	{
		if (response.location_found && appendIndexFile(response.path, loc))
		{
			// response.uploadDir = substr_before_rdel(response.path, "/");
			response.uploadFile = substr_after_rdel(response.path, "/");
			std::cout << "response.uploadFile2 " << response.uploadFile << std::endl;
			if (is_regular_file(response.path))
			{
				if (!is_matching_ext(response.path, loc.getCgiExtension()))
				{
					return 1;
				}
				response.uploadFile = substr_after_rdel(response.path, "/");
				std::cout << "response.uploadFile3 " << response.uploadFile << std::endl;
				// setCGIResponse(response, OK);
				setGetResponse(response, OK);
				return 0;
			}
			else if (!is_directory(response.path))
			{
				return 3;
			}
			else
			{
				return 5;
			}
		}
		else
		{
			return 2;
		}
	}
	return 4;
}