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

	EXPECT_EQ(typeid(value), typeid(expected));
	EXPECT_DECIMAL_EQ(value, expected);
}

TEST(DecimalType, UserDefLiteral_Bench)
{
	for (int i = 0; i++ < 100000;)
		maths::Decimal value = 1.1_d;
}

TEST(UtilityFunctions, Abs)
{
	maths::Decimal d_one = 1._d;
	float f32_one = 1.f;
	double f64_one = 1.0;
	int32_t i32_one = 1;
	int64_t i64_one = 1;

	EXPECT_EQ(maths::Abs(d_one), d_one);
	EXPECT_EQ(maths::Abs(-d_one), d_one);

	EXPECT_EQ(maths::Abs(f32_one), f32_one);
	EXPECT_EQ(maths::Abs(-f32_one), f32_one);

	EXPECT_EQ(maths::Abs(f64_one), f64_one);
	EXPECT_EQ(maths::Abs(-f64_one), f64_one);

	EXPECT_EQ(maths::Abs(i32_one), i32_one);
	EXPECT_EQ(maths::Abs(-i32_one), i32_one);

	EXPECT_EQ(maths::Abs(i64_one), i64_one);
	EXPECT_EQ(maths::Abs(-i64_one), i64_one);
}


typedef ::testing::Types<maths::Decimal, float, double, int32_t, int64_t, uint32_t, uint64_t> numeric_types;
template <typename T>
class PositiveNumericTest : public ::testing::Test
{
protected:
	PositiveNumericTest() :
		A{ 1 }, B{ 2 }, C{ 3 }
	{}

	T A, B, C;
};
TYPED_TEST_CASE(PositiveNumericTest, numeric_types);

TYPED_TEST(PositiveNumericTest, MinMax)
{
	EXPECT_EQ(maths::Min(A, B), A);
	EXPECT_EQ(maths::Max(A, B), B);
}

TYPED_TEST(PositiveNumericTest, Clamp)
{
	EXPECT_EQ(maths::Clamp(C, A, B), B);
	EXPECT_EQ(maths::Clamp(C, B, A), A);
	EXPECT_EQ(maths::Clamp(A, B, C), B);
	EXPECT_EQ(maths::Clamp(A, C, B), B);
	EXPECT_EQ(maths::Clamp(B, A, C), B);
	EXPECT_EQ(maths::Clamp(B, C, A), A);

	EXPECT_EQ(maths::SafeClamp(C, A, B), B);
	EXPECT_EQ(maths::SafeClamp(C, B, A), B);
	EXPECT_EQ(maths::SafeClamp(A, B, C), B);
	EXPECT_EQ(maths::SafeClamp(A, C, B), B);
	EXPECT_EQ(maths::SafeClamp(B, A, C), B);
	EXPECT_EQ(maths::SafeClamp(B, C, A), B);
}

