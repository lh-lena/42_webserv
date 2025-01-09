/**
 * to compile:
 *  g++ -std=c++11 -lgtest -lgtest_main HandleRequestedURITest.cpp ../srcs/Server.cpp -o server_test
 * to execute:
 *  ./server_test
 */
#include "gtest/gtest.h"
#include "../includes/ParseConfig.hpp"
int main(int argc, char **argv, char **envp)
{
	(void)argc;
	(void)argv;
	std::string configFilePath;
	configFilePath = "serv.conf";
	ParseConfig config(configFilePath, envp);
	config.readFileContent();
	config.parseConfigContent();

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
