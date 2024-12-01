#ifndef PARSECONFIG_HPP
# define PARSECONFIG_HPP

# include <iostream>
# include <fstream>
# include <map>
# include <vector>
# include <sstream>
# include <cstring>
# include <sys/stat.h>
# include <exception>

class ParseConfig
{
private:
    char                        **_envp;
    std::vector<std::string>    _http_directives;
    std::vector<std::string>    _server_directives;
    std::vector<std::string>    _location_directives;

public:
	ParseConfig(char **envp);
	~ParseConfig();



    void setHttpDirective(const std::string &directive);
    void setServerDirective(const std::string &directive);
    void setLocationDirective(const std::string &directive);
    void parseConfigFile(std::string& filePath);
    static bool isDirectory(const std::string& path);
    static std::string getEnvValue(char **envp, std::string &variable);

    class ParseException : public std::exception
    {
        public:
            ParseException(const std::string &message);
            const char *what() const throw();
            ~ParseException() throw();
        private:
            std::string _msg;
    };
};

#endif