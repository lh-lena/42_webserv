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
		// Location( Location const & src );
		~Location();

		// Location &		operator=( Location const & rhs );


	private:

		bool								_autoindex;
		int									_timeout;
		std::string							_client_max_body_size;
		std::string							_name;
		std::string							_root;
		std::string							_upload_dir;
		std::string							_cgi_extension;
		std::vector<std::string>			_indexes;
		std::vector<std::string>			_allowed_methods;
		std::map<std::string, std::string>	_error_pages;
		std::map<std::string, std::string>	_redir;
};

// std::ostream &			operator<<( std::ostream & o, Location const & i );

#endif /* ******************************************************** LOCATION_H */