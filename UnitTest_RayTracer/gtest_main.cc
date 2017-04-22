#include <cstdio>
#include "gtest/gtest.h"

GTEST_API_ int main(int _argc, char **_argv)
{
	testing::InitGoogleTest(&_argc, _argv);
	int result = RUN_ALL_TESTS();

	std::getchar();
	return result;
}