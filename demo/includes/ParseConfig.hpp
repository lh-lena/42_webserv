#ifndef PARSECONFIG_HPP
# define PARSECONFIG_HPP

# include <map>
# include <list>
# include <vector>
# include <string>
# include <fstream>
# include <sstream>
# include <cstring>
# include <iostream>
# include <iterator>
# include <sys/stat.h>
# include <exception>
# include "Server.hpp"
# include "Location.hpp"

# define BLOCK "block"

class ParseConfig
{
	public:
		ParseConfig(std::string file_path, char **envp);
		~ParseConfig();

		void				setGlobalDirective(const std::string &directive, bool required);
		void				setHttpDirective(const std::string &directive, bool required);
		void				setServerDirective(const std::string &directive, bool required);
		void				setLocationDirective(const std::string &directive, bool required);
		void				readFileContent( void );
		void				parseConfigContent( void );
		static bool			isDirectory(const std::string& path);
		void				printConfigContent( void );
		int					exceptTocken(std::list<std::string> *src, std::string tocken);

		static std::string	getEnvValue(char **envp, const std::string &variable);
		
		class ParseException : public std::exception
		{
			public:
				ParseException(const std::string &message);
				const char *what() const throw();
				~ParseException() throw();
			private:
				std::string	_msg;
		};
	
	private:
		char							**_envp;
		std::string						_conf_file_path;
		std::list<std::string>			_conf_content;
		std::map<std::string, bool>		_global_directives;
		std::map<std::string, bool>		_http_directives;
		std::map<std::string, bool>		_server_directives;
		std::map<std::string, bool>		_location_directives;
		std::vector<Server>				_servers;
};

#endif /* ******************************************************** PARSECONFIG_HPP */