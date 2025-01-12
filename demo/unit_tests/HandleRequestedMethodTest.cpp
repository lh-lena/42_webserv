/*
Request req;
Response resp;
req.method_r = "GET";
req.reqURI = extractPath(request);
Server serv = getServers()[0];
serv.handleRequestMethod(req, resp);
serv.createResponse(resp, str);
*/
/**
 * to compile:
 *  g++ -std=c++11 -lgtest -lgtest_main HandleRequestedURITest.cpp ../srcs/Server.cpp -o server_test
 * to execute:
 *  ./server_test
 *  # Needs google-test
    # Needs google-test/gtest.h
 */

#include "gtest/gtest.h"
#include "../includes/Server.hpp"
#include "../includes/Location.hpp"
#include "../includes/ParseConfig.hpp"
#include "../includes/ServerControler.hpp"
#include "../includes/generate_html.hpp"

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

