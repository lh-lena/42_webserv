/**
 * to compile:
 *  g++ -std=c++11 -lgtest -lgtest_main HandleRequestedURITest.cpp ../srcs/Server.cpp -o server_test
 * to execute:
 *  ./server_test
 */
#include "gtest/gtest.h"

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
