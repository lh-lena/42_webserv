#include "gtest/gtest.h"
#include "../includes/Server.hpp"
#include "../includes/Location.hpp"
#include "../includes/CGI.hpp"
#include "../includes/Request.hpp"
#include "../includes/ParseConfig.hpp"
#include "../includes/ServerControler.hpp"
#include "../includes/utils.hpp"

Server server;


std::string request =   "POST /cgi-bin/contact_handler.php HTTP/1.1
                    Host: localhost:8080
                    Connection: keep-alive
                    Content-Length: 73
                    sec-ch-ua-platform: \"macOS\"
                    User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/131.0.0.0 Safari/537.36
                    Content-Type: application/json
                    sec-ch-ua-mobile: ?0
                    Accept: */*
                    Origin: [http://localhost:8080](http://localhost:8080/)
                    Sec-Fetch-Site: same-origin
                    Sec-Fetch-Mode: cors
                    Sec-Fetch-Dest: empty
                    Referer: http://localhost:8080/contact_us/index.html
                    Accept-Encoding: gzip, deflate, br, zstd
                    Accept-Language: uk-UA,uk;q=0.9,en-US;q=0.8,en;q=0.7,ru;q=0.6

                    \"Hello\"


"

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

TEST(CGITest, setEnvironment)
{
    Request req;
    req.parse(request);
    
    CGI cgi();

    cgi.setEnvironment(req);
    cgi.printEnvironment();

};
