#ifndef REQUESTHANDLER_HPP
# define REQUESTHANDLER_HPP

# include "Request.hpp"
# include "Response.hpp"
# include "Server.hpp"
# include "Location.hpp"

class RequestHandler
{
public:
	RequestHandler(const Server& server, const Request& request, Response& response);
	~RequestHandler();
	void processRequest();

	static std::string		canonicalizePath(const std::string& path);
	static std::string		decodeURI(const std::string& path);

	std::string		_path;

private:
	const Server&	_server;
	const Request&	_request;
	Response&		_response;
	Location		_location;

	bool					isRedirection( void ) const;
	void					setRedirectResponse( void );
	bool					isCGIRequest( void ) const;
	bool					isMethodAllowed( void ) const;
	bool					isImplementedMethod( void ) const;
	bool					findRequestedLocation(const std::string& path);
	bool					searchingExtensionMatchLocation(const std::string& requested_path);
	bool					searchingPrefixMatchLocation(const std::string& requested_path);
	void					setErrorResponse(int status_code);
	std::string				getCustomErrorPage(int status_code);
	std::string				determineFilePath(const std::string& requested_path);
	void					handleStaticRequest( void );
	void					handleGET( void );
	void					setGetResponse(int status_code);
	void					setDirectoryListingResponse(int status_code);
	void					handleGetDirectoryResponse( void );
	bool					appendIndexFile( void );
	void					handlePOST( void );
	std::string				searchingUploadDir( void );
	void					handleDELETE( void );
	int						remove_file(const std::string& path);
	int						remove_directory(const std::string& path);
	int						handleDeleteDirectoryResponse( void );
};

#endif /** REQUESTHANDLER_HPP */
