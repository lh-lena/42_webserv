#ifndef LOCATION_HPP
# define LOCATION_HPP

# include <map>
# include <string>
# include <vector>
# include <iostream>

class Location
{

	public:

		Location();
		Location( Location const & src );
		~Location();

		Location &		operator=( Location const & rhs );

		void								setPath(const std::string &arg);
		void								setRoot(const std::string &arg);
		void								setIndex(const std::string &arg);
		void								setAutoindex(bool arg);
		void								setAllowedMethod(const std::string &arg);
		void								setUploadDir(const std::string &arg);
		void								setErrorPages(const std::string &key, const std::string &val);
		void								setRedirect( const std::string &key, const std::string &val );
		void								setCgiExtension(const std::string &arg);
		bool								getAutoindex( void ) const;
		std::string							getPath( void ) const;
		std::string							getRoot( void ) const;
		std::string							getUploadDir( void ) const;
		std::string							getCgiExtension( void ) const;
		std::vector<std::string>			getIndexes( void ) const;
		std::vector<std::string>			getAllowedMethods( void ) const;
		std::map<std::string, std::string>	getErrorPages( void ) const;
		std::map<std::string, std::string>	getRedirect( void ) const;

	private:

		bool								_autoindex;
		std::string							_path;
		std::string							_root;
		std::string							_upload_dir;
		std::string							_cgi_extension;
		std::vector<std::string>			_indexes;
		std::vector<std::string>			_allowed_methods;
		std::map<std::string, std::string>	_error_pages;
		std::map<std::string, std::string>	_redir;
};

std::ostream &			operator<<( std::ostream & o, Location const & i );

#endif /* ******************************************************** LOCATION_HPP */