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

		typedef void		(ParseConfig::*DirectiveHandler)(const std::string&, void*);
		void				setGlobalDirective(const std::string &directive, DirectiveHandler handler);
		void				setHttpDirective(const std::string &directive, DirectiveHandler handler);
		void				setServerDirective(const std::string &directive, DirectiveHandler handler);
		void				setLocationDirective(const std::string &directive, DirectiveHandler handler);
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
		char										**_envp;
		std::string									_conf_file_path;
		std::vector<Server>							_servers;
		std::list<std::string>						_conf_content;
		std::map<std::string, DirectiveHandler>		_global_directives;
		std::map<std::string, DirectiveHandler>		_http_directives;
		std::map<std::string, DirectiveHandler>		_server_directives;
		std::map<std::string, DirectiveHandler>		_location_directives;

		template<typename T> void	handleWorkCont(const std::string& value, T* instance);
		template<typename T> void	handleHttpBlock(const std::string& value, T* instance);
		template<typename T> void	handleServerBlock(const std::string& value, T* instance);
};

#endif /* ******************************************************** PARSECONFIG_HPP */