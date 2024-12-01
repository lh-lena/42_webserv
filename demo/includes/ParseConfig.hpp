#ifndef PARSECONFIG_HPP
# define PARSECONFIG_HPP

# include <map>
# include <vector>
# include <string>
# include <fstream>
# include <sstream>
# include <cstring>
# include <iostream>
# include <sys/stat.h>
# include <exception>

class ParseConfig
{
    public:
        ParseConfig(char **envp);
        ~ParseConfig();

        void                setGlobalDirective(const std::string &directive);
        void                setHttpDirective(const std::string &directive);
        void                setServerDirective(const std::string &directive);
        void                setLocationDirective(const std::string &directive);
        void                readFileContent(std::string& filePath);
        static bool         isDirectory(const std::string& path);
        static std::string  getEnvValue(char **envp, const std::string &variable);

        class ParseException : public std::exception
        {
            public:
                ParseException(const std::string &message);
                const char *what() const throw();
                ~ParseException() throw();
            private:
                std::string _msg;
        };
    
    private:
        char                        **_envp;
        std::vector<std::string>    _contentConfFile;
        std::vector<std::string>    _global_directives;
        std::vector<std::string>    _http_directives;
        std::vector<std::string>    _server_directives;
        std::vector<std::string>    _location_directives;
};

#endif /* ******************************************************** PARSECONFIG_HPP */