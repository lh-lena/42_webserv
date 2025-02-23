#ifndef REQUESTHANDLER_HPP
# define REQUESTHANDLER_HPP

# include "Request.hpp"
# include "Response.hpp"
# include "Server.hpp"
# include "Location.hpp"

/** HttpStatusCode */
# define OK 200
# define CREATED 201
# define NO_CONTENT 204
# define MOVED_PERMANENTLY 301
# define MOVED_TEMPORARY 302
# define BAD_REQUEST 400
# define FORBIDDEN 403
# define FORBIDDEN_DIR 403
# define NOT_FOUND 404
# define CONFLICT 409
# define REQUEST_ENTITY_TOO_LARGE 413
# define METHOD_NOT_ALLOWED 405
# define NOT_IMPLEMENTED 501
# define INTERNAL_SERVER_ERROR 500

class RequestHandler
{
public:
	RequestHandler(const Server& server, Request& request, Response& response);
	~RequestHandler();
	void processRequest();

	static std::string		canonicalizePath(const std::string& path);
	static std::string		normalizePath(const std::string& path);
	static std::string		decodeURI(const std::string& path);
	static std::string		generateHtmlErrorPage( int status_code);

private:
	const Server&			_server;
	Request&				_request;
	Response&				_response;
	Location				_location;

	bool					isRedirection( void ) const;
	void					setRedirectResponse( void );
	bool					isCGIRequest( void ) const;
	void					handleCgiRequest( void );
	void					handleCgiResponse(const std::string& data);
	bool					isMethodAllowed( void ) const;
	bool					isImplementedMethod( void ) const;
	bool					findRequestedLocation(const std::string& path);
	bool					searchingExtensionMatchLocation(const std::string& requested_path);
	bool					searchingPrefixMatchLocation(const std::string& requested_path);
	bool					isExternalRedirect(const std::string& path);
	void					setCustomErrorResponse(int status_code, const std::string& custom_error_path);
	std::string				getCustomErrorPath(int status_code);
	std::string				determineFilePath(const std::string& requested_path);
	void					handleStaticRequest( void );
	void					setDirectoryListingResponse(int status_code);
	void					handleGetDirectoryResponse( void );
	std::string				appendIndexFile( const std::string& path );
	void					handlePOST( void );
	std::string				searchingUploadPath( void );
	void					handleDELETE( void );
	int						remove_file(const std::string& path);
	int						remove_directory_recursively(const std::string& path);
	int						handleDeleteDirectoryResponse( void );
	std::string				generateHtmlDirectoryListing(const std::string& path);
};

#endif /** REQUESTHANDLER_HPP */
