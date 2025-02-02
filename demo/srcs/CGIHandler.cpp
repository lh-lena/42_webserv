
#include "../includes/CGIHandler.hpp"
#include "../includes/Server.hpp"

// CGIHandler::CGIHandler(const Server& server, const Location& loc) : 
// {}


void CGIHandler::setEnvironment(const Request& request)
{
	env["AUTH_TYPE"] = std::string();
	// env["CONTENT_TYPE"] = request.contentType;
	env["CONTENT_LENGTH"] = request.contentLength;
	env["GATEWAY_INTERFACE"] = "CGI/1.1";
	// env["HTTP_ACCEPT_CHARSET"] = request.charset;
	// env["PATH_INFO"] = request.pathInfo; // the portion of the URI path following the script name but preceding any query data.
	env["PATH_TRANSLATED"] = std::string();
	env["QUERY_STRING"] = request.query;
	env["REMOTE_ADDR"] = std::string(); // Returns the IP address of the client that sent the request
	env["REMOTE_HOST"] = std::string(); // the fully-qualified name of the client that sent the request, or the IP address of the clien
	env["REMOTE_IDENT"] = std::string();
	env["REMOTE_USER"] = std::string();
	env["REQUEST_METHOD"] = request.method;
	env["SCRIPT_NAME"] = std::string();
	env["SERVER_NAME"] = std::string();
	env["SERVER_PORT"] = std::string();
	env["SERVER_PROTOCOL"] = request.protocol;
	env["SERVER_SOFTWARE"] = std::string();
	env["SERVER_ROOT"] = std::string();
}

/** NOTES:
 * - parse cgi request
 * - path also a cgi path to the executable file without query
 * - 
 */

int		Server::handleCGI(Response& response, Location& loc)
{
	std::cout << "response.status_code " << response.status_code << std::endl;

	response.uploadDir = utils::substr_before_rdel(response.path, "/");
	std::cout << "response.uploadDir1 " << response.uploadDir << std::endl;
	response.uploadDir = Server::searchingUploadDir(response.reqURI, &loc);

	std::cout << "response.uploadDir2 " << response.uploadDir << std::endl;

	std::cout << "response.path1 " << response.path << std::endl;
	if (response.path.find("?") != std::string::npos) /* check queries */
	{
		response.query = utils::substr_after_rdel(response.path, "?");
		response.path = utils::substr_before_rdel(response.path, "?");
	}
	std::cout << "is_regular_file(response.path) " << utils::is_regular_file(response.path) << std::endl;
	if (utils::is_regular_file(response.path))
	{
		std::cout << "is_matching_ext(response.path, loc->getCgiExtension()) " << utils::is_matching_ext(response.path, loc.getCgiExtension()) << std::endl;
		if (!response.location_found || !utils::is_matching_ext(response.path, loc.getCgiExtension()))
		{
			return 1;
		}
		response.uploadFile = utils::substr_after_rdel(response.path, "/");
		std::cout << "response.uploadFile1 " << response.uploadFile << std::endl;
		// setCGIResponse(response, OK);
		setGetResponse(response, OK);
		return 0;
	}
	else if (!utils::is_directory(response.path))
	{
		if (response.location_found && appendIndexFile(response.path, loc))
		{
			// response.uploadDir = substr_before_rdel(response.path, "/");
			response.uploadFile = utils::substr_after_rdel(response.path, "/");
			std::cout << "response.uploadFile2 " << response.uploadFile << std::endl;
			if (utils::is_regular_file(response.path))
			{
				if (!utils::is_matching_ext(response.path, loc.getCgiExtension()))
				{
					return 1;
				}
				response.uploadFile = utils::substr_after_rdel(response.path, "/");
				std::cout << "response.uploadFile3 " << response.uploadFile << std::endl;
				// setCGIResponse(response, OK);
				setGetResponse(response, OK);
				return 0;
			}
			else if (!utils::is_directory(response.path))
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