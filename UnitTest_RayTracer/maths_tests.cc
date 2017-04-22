#include "gtest/gtest.h"

#include "global_definitions.h"

TEST(DecimalType, IsExpectedType)
{
	maths::Decimal	value = 1.1_d;
#ifdef YS_DECIMAL_IS_DOUBLE
	double			expected = 1.1;
#else
	float			expected = 1.1f;
#endif

	ASSERT_DECIMAL_EQ(value, expected);
}

TEST(DecimalType, UserDefLiteral_Bench)
{
	for (int i = 0; i++ < 100000;)
	{
		maths::Decimal value = 1.1_d;
	}
}

