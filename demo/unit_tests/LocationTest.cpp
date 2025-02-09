#include "gtest/gtest.h"
#include "../includes/Server.hpp"
#include "../includes/Location.hpp"
#include "../includes/Request.hpp"
#include "../includes/Response.hpp"
#include "../includes/ParseConfig.hpp"
#include "../includes/ServerControler.hpp"
#include "../includes/RequestHandler.hpp"
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

TEST(LocationTest, searchingExtensionMatchLocation)
{
        std::ostringstream request1;
        request1 << "GET /cgi_bin/test.py HTTP/1.1"
                    << "Host: localhost:8080"
                    << "Connection: keep-alive"
                    << "Content-Length: 73"
                    << "Hello";
    Response response;
    Request req;

    req.parse(request1.str());
    RequestHandler reqHandle(server, req, response);

    reqHandle.searchingPrefixMatchLocation("/cgi_bin/test.py");
    std::cout << reqHandle._location << std::endl;
}

TEST(LocationTest, searchingPrefixMatchLocation)
{
        std::ostringstream request1;
        request1 << "GET /upload/test.py HTTP/1.1"
                    << "Host: localhost:8080"
                    << "Connection: keep-alive"
                    << "Content-Length: 73"
                    << "Hello";

    Response response;
    Request req;

    req.parse(request1.str());
    RequestHandler reqHandle(server, req, response);

    reqHandle.searchingPrefixMatchLocation("/upload/test.py");
    std::cout << reqHandle._location << std::endl;
}

TEST(LocationTest, determineFilePath)
{
    Response response;
    Request req;

    std::ostringstream request1;
        request1 << "GET /upload/test.py HTTP/1.1"
                    << "Host: localhost:8080"
                    << "Connection: keep-alive"
                    << "Content-Length: 73"
                    << "Hello";

    req.parse(request1.str());
    RequestHandler reqHandle(server, req, response);

    std::string test1 = "/srcs/index.html";
    std::string res1 = reqHandle.determineFilePath(test1);
    std::string expected1 = "var/www/html/wordle" + test1;

    EXPECT_EQ(res1, expected1);

    std::string test2 = "/test/index.html";
    std::string res2 = reqHandle.determineFilePath(test2);
    std::string expected2 = "var/www/html/wordle/index.html";

    EXPECT_EQ(res2, expected2);
}

