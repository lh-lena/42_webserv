#include "gtest/gtest.h"
#include "../includes/Server.hpp"
#include "../includes/Location.hpp"
#include "../includes/CGI.hpp"
#include "../includes/Request.hpp"
#include "../includes/ParseConfig.hpp"
#include "../includes/ServerControler.hpp"
#include "../includes/utils.hpp"

Server server;

void  initializeServer(char **envp)
{
  try
  {
    std::string configFilePath;
    configFilePath = "unit_tests/serv_test.conf";
    ParseConfig config(configFilePath, envp);
    
    config.readFileContent();
    config.parseConfigContent();

    ServerControler & controler = config.getServControler();
    server = controler.getServers()[0];
  }
  catch(const std::exception& e)
  {
    std::cerr << e.what() << '\n';
  }

}

void init_req(Request& req)
{
    req.contentLength = 12;
    req.method = "GET";
    req.protocol = "HTTP/1.1";
    req.query = "&a=1&b=2";
}

TEST(CGITest, setEnvironment)
{
    Request req;
    init_req(req);
    CGI cgi(req);

    cgi.setEnvironment(req);
    cgi.printEnvironment();

};
