#ifndef PARSECONFIG_HPP
# define PARSECONFIG_HPP

# include <map>
# include <list>
# include <vector>
# include <string>
# include <fstream>
# include <sstream>
# include <cstring>
# include <utility>
# include <iostream>
# include <iterator>
# include <sys/stat.h>
# include <exception>
# include "Server.hpp"
# include "Location.hpp"
# include "ServerControler.hpp"

class ParseConfig
{
	public:
		ParseConfig(std::string file_path, char **envp);
		~ParseConfig();

		typedef void					(ParseConfig::*DirectiveServerHandler)(const std::pair<std::string, int>&, Server*);
		typedef void					(ParseConfig::*DirectiveLocationHandler)(const std::pair<std::string, int>&, Location*);
		void							handleHttpBlock(const std::pair<std::string, int>& value, Server* instance);
		void							handleServerBlock(const std::pair<std::string, int>& value, Server* instance);
		void							handleLocationBlock(const std::pair<std::string, int>& value, Location* instance);
		void							handleClientBodySize(const std::pair<std::string, int>& value, Server* instance);
		void							handleListen(const std::pair<std::string, int>& value, Server* instance);
		void							handleWorkCont(const std::pair<std::string, int>& value, Server* instance);
		void							handleServerName(const std::pair<std::string, int>& value, Server* instance);
		void							handleErrorLog(const std::pair<std::string, int>& value, Server* instance);
		void							handleAllowedMethods(const std::pair<std::string, int>& value, Location* instance);
		void							handleAutoindex(const std::pair<std::string, int>& value, Location* instance);
		void							handleUploadDir(const std::pair<std::string, int>& value, Location* instance);
		void							handlePath(const std::pair<std::string, int>& value, Location* instance);
		void							handleReturn(const std::pair<std::string, int>& value, Location* instance);
		void							handleCgiExtension(const std::pair<std::string, int>& value, Location* instance);
		void							setGlobalDirective(const std::string &directive, DirectiveServerHandler handler);
		void							setHttpDirective(const std::string &directive, DirectiveServerHandler handler);
		void							setServerDirective(const std::string &directive, DirectiveServerHandler handler);
		void							setLocationDirective(const std::string &directive, DirectiveLocationHandler handler);
		void							readFileContent( void );
		void							parseConfigContent( void );
		int								exceptTocken(std::list<std::pair<std::string, int> > *src, std::pair<std::string, int> tocken, int expected);
		std::string						processEnvVar(const std::string &input);
		std::pair<std::string, int> 	getToken(std::list<std::pair<std::string, int> > *src);
		ServerControler & 				getServControler();

		std::map<std::string, bool>		block_dir;
		std::map<std::string, bool>		map_template_dir;
		std::map<std::string, bool>		allowed_methods;

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
		ParseConfig( ParseConfig const & src ); // not implemented
		ParseConfig &		operator=( ParseConfig const & rhs ); // not implemented
		ServerControler										_serverControler;
		char												**_envp;
		std::string											_conf_file_path;
		std::list<std::pair<std::string, int> >				_conf_content;
		std::map<std::string, DirectiveServerHandler>		_global_directives;
		std::map<std::string, DirectiveServerHandler>		_http_directives;
		std::map<std::string, DirectiveServerHandler>		_server_directives;
		std::map<std::string, DirectiveLocationHandler>		_location_directives;

		template<typename T> void	handleRoot(const std::pair<std::string, int>& value, T* instance);
		template<typename T> void	handleIndex(const std::pair<std::string, int>& value, T* instance);
		template<typename T> void	handleErrorPage(const std::pair<std::string, int>& value, T* instance); // to change
};


#endif /* ******************************************************** PARSECONFIG_HPP */
