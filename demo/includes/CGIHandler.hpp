#ifndef CGIHANDLER_HPP

# include "Request.hpp"
# include "Response.hpp"
# include "Location.hpp"

class CGIHandler
{
public:
    CGIHandler();
    ~CGIHandler();

    void    setVariables(const Request& request);
    // int		handleCGI(Response& response, Location& loc);

private:
    std::map<std::string, std::string>  env;
};

#endif /* CGIHANDLER_HPP */