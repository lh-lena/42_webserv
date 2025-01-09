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
#include "../includes/generate_html.hpp"

TEST(ServerTest, HandleRequestedURI)
{
  Server server; 

  std::string root = "var/www/html";
  std::string index = "index.html";

  // Test case 1: Simple path
  std::string base_path1 = "/" + index;
  std::string path1 = "";
  std::string expected_path1 = root + base_path1;
  server.handleRequestedURI(base_path1, path1);
  EXPECT_EQ(path1, expected_path1);

  // Test case 2: Path with leading slash
  std::string base_path2 = "/wordle/";
  std::string path2 = "";
  std::string expected_path2 = root + base_path2;
  server.handleRequestedURI(base_path2, path2);
  EXPECT_EQ(path2, expected_path2);

  // Test case 3: Path without leading slash
  std::string base_path3 = "/about/us";
  std::string path3 = "";
  std::string expected_path3 = root + base_path3;
  server.handleRequestedURI(base_path3, path3);
  EXPECT_EQ(path3, expected_path3);

  // Test case 4: Empty base path
  std::string base_path4 = "";
  std::string path4 = "";
  std::string expected_path4 = root + "/index.html";
  server.handleRequestedURI(base_path4, path4);
  EXPECT_EQ(path4, expected_path4);

  // Test case 5: Empty path
  std::string base_path5 = "/var/www";
  std::string path5 = "";
  std::string expected_path5 = "404.html"; 
  server.handleRequestedURI(base_path5, path5);
  EXPECT_EQ(path5, expected_path5); 
}