#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <map>
# include <string>
# include <vector>
# include <iostream>
# include "utils.hpp"

class Location
{

public:

	Location();
	Location( Location const & src );
	~Location();

	Location &		operator=( Location const & rhs );

	void										defaultRootLocation( void );
	void										setAutoindex(bool arg);
	void										setClientMaxBody(size_t value);
	void										setPath(const std::string &arg);
	void										setRoot(const std::string &arg);
	void										setAlias(const std::string &arg);
	void										setIndex(const std::string &arg);
	void										setUploadDir(const std::string &arg);
	void										setCGIExtension(const std::string &arg);
	void										setCgiInterpreter(const std::string &key, const std::string& val);
	void										setAllowedMethod(const std::string &arg);
	void										addErrorPage(int key, const std::string &val);
	void										setRedirect( int key, const std::string &val );
	bool										getAutoindex( void ) const;
	size_t										getClientMaxBody( void ) const;
	const std::string&							getPath( void ) const;
	const std::string&							getRoot( void ) const;
	const std::string&							getAlias( void ) const;
	const std::string&							getUploadDir( void ) const;
	const std::map<std::string, std::string>&	getCgiInterpreter( void ) const;
	const std::vector<std::string>&				getCGIExtension( void ) const;
	const std::vector<std::string>&				getIndexes( void ) const;
	const std::vector<std::string>&				getAllowedMethods( void ) const;
	const std::map<int, std::string>&			getErrorPages( void ) const;
	const std::map<int, std::string>&			getRedirect( void ) const;

private:

	bool										_autoindex;
	size_t										_client_max_body_size;
	std::string									_path;
	std::string									_root;
	std::string									_alias;
	std::string									_upload_dir;
	std::vector<std::string>					_indexes;
	std::vector<std::string>					_method;
	std::vector<std::string>					_cgi_extension;
	std::map<std::string, std::string>			_cgi_interpreter;
	std::map<int, std::string>					_error_pages;
	std::map<int, std::string>					_return;
};

std::ostream &			operator<<( std::ostream & o, Location const & i );

#endif