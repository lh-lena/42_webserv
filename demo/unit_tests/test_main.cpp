/**
 * to compile:
 *  g++ -std=c++11 -lgtest -lgtest_main HandleRequestedURITest.cpp ../srcs/Server.cpp -o server_test
 * to execute:
 *  ./server_test
 */
#include "gtest/gtest.h"
#include "../includes/ParseConfig.hpp"

void  initializeServer(char **envp);

int main(int argc, char **argv, char **envp)
{
	(void)argc;
	(void)argv;

	initializeServer(envp);

	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
