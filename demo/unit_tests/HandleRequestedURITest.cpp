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

TEST(ServerTest, HandleRequestedURI)
{
  Location loc;
  bool						found_loc;
  std::string root = "var/www/html";
  std::string index = "index.html";

  // Test case 1: Simple path
  std::string base_path1 = "/" + index;
  std::string path1 = "";
  std::string expected_path1 = root + base_path1;
  server.handleRequestedURI(base_path1, path1, loc, found_loc);
  EXPECT_EQ(path1, expected_path1);

  // Test case 2: Path with leading slash
  std::string base_path2 = "/wordle/";
  std::string path2 = "";
  std::string expected_path2 = root + base_path2 + index;
  server.handleRequestedURI(base_path2, path2, loc, found_loc);
  EXPECT_EQ(path2, expected_path2);

  // Test case 3: Path without leading slash and invalid path
  std::string base_path3 = "/about/us";
  std::string path3 = "";
  std::string expected_path3 = generate_html_error_page(404);
  server.handleRequestedURI(base_path3, path3, loc, found_loc);
  EXPECT_EQ(path3, expected_path3);

  // Test case 4: Empty base path
  std::string base_path4 = "";
  std::string path4 = "";
  std::string expected_path4 = root + "/index.html";
  server.handleRequestedURI(base_path4, path4, loc, found_loc);
  EXPECT_EQ(path4, expected_path4);

  // Test case 5: Invalid path
  std::string base_path5 = "/var/www";
  std::string path5 = "";
  std::string expected_path5 = generate_html_error_page(404);
  server.handleRequestedURI(base_path5, path5, loc, found_loc);
  EXPECT_EQ(path5, expected_path5);

  // Test case 6: Invalid path
  std::string base_path6 = "/srcs/";
  std::string path6 = "";
  std::string expected_path6 = root + "/wordle/srcs/index.js"; 
  server.handleRequestedURI(base_path6, path6, loc, found_loc);
  EXPECT_EQ(path6, expected_path6);

  // Test case 7: Invalid path
  std::string base_path7 = "srcs";
  std::string path7 = "";
  std::string expected_path7 = generate_html_error_page(404);
  server.handleRequestedURI(base_path7, path7, loc, found_loc);
  EXPECT_EQ(path7, expected_path7);

  // Test case 8: No leading '/' in a directory
  std::string base_path8 = "/wordle";
  std::string path8 = "";
  std::string expected_path8 = generate_html_error_page(301);
  server.handleRequestedURI(base_path8, path8, loc, found_loc);
  EXPECT_EQ(path8, expected_path8);

  // Test case 9: Valid redirect
  std::string base_path9 = "/redir/";
  std::string path9 = "";
  std::string expected_path9 = "var/www/error_pages/405.html";
  server.handleRequestedURI(base_path9, path9, loc, found_loc);
  EXPECT_EQ(path9, expected_path9);

  // Test case 10: invalid path of the redirect
  std::string base_path10 = "/inv_redir/";
  std::string path10 = "";
  std::string expected_path10 = generate_html_error_page(405);
  server.handleRequestedURI(base_path10, path10, loc, found_loc);
  EXPECT_EQ(path10, expected_path10);


}

TEST(ServerTest, getCustomErrorPage)
{
  std::vector<Location> locations = server.getLocations();

  int st_code1 = 405;
  std::string path1 = server.getCustomErrorPage(st_code1, locations[0]);
  std::string expected_path1 = "var/www/error_pages/405.html";
  EXPECT_EQ(path1, expected_path1);
  
  int st_code2 = 404;
  std::string path2 = server.getCustomErrorPage(st_code2, locations[0]);
  std::string expected_path2 = generate_html_error_page(st_code2);
  EXPECT_EQ(path2, expected_path2);
  
}

