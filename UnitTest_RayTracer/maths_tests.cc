#include "gtest/gtest.h"

#include "global_definitions.h"
#include "redecimal.h"

#ifdef YS_DECIMAL_IS_DOUBLE
#define DECIMAL_SHIFT 53
#else
#define DECIMAL_SHIFT 24
#endif

TEST(DecimalType, ConstantsMasks)
{
	maths::DecimalBits full_mask =
		maths::DecimalMeta::exponent_mask ^ maths::DecimalMeta::significand_mask ^ maths::DecimalMeta::sign_mask;
	maths::DecimalBits validation = maths::DecimalBits(0) - 1;
	EXPECT_EQ(full_mask, validation);
}

TEST(DecimalType, Infinity)
{
	maths::DecimalBits	infinity_a = maths::DecimalMeta::exponent_mask;
	maths::DecimalBits	infinity_b = maths::DecimalMeta::sign_mask | maths::DecimalMeta::exponent_mask;
	maths::DecimalBits	nan_c = maths::DecimalMeta::sign_mask | maths::DecimalMeta::exponent_mask | 1U;
	maths::Decimal		not_infinity_d = 3._d;

	auto mapper_a = maths::DecimalBitsMapper(infinity_a);
	auto mapper_b = maths::DecimalBitsMapper(infinity_b);
	auto mapper_c = maths::DecimalBitsMapper(nan_c);

	EXPECT_TRUE(maths::IsInf(mapper_a.value));
	EXPECT_TRUE(maths::IsInf(mapper_b.value));
	EXPECT_FALSE(maths::IsInf(mapper_c.value));
	EXPECT_FALSE(maths::IsInf(not_infinity_d));
}

TEST(DecimalType, NaN)
{
	maths::DecimalBits	infinity_a = maths::DecimalMeta::exponent_mask;
	maths::DecimalBits	infinity_b = maths::DecimalMeta::sign_mask | maths::DecimalMeta::exponent_mask;
	maths::DecimalBits	nan_c = maths::DecimalMeta::sign_mask | maths::DecimalMeta::exponent_mask | 1U;
	maths::Decimal		not_infinity_d = 3._d;

	auto mapper_a = maths::DecimalBitsMapper(infinity_a);
	auto mapper_b = maths::DecimalBitsMapper(infinity_b);
	auto mapper_c = maths::DecimalBitsMapper(nan_c);

	EXPECT_FALSE(maths::IsNaN(mapper_a.value));
	EXPECT_FALSE(maths::IsNaN(mapper_b.value));
	EXPECT_TRUE(maths::IsNaN(mapper_c.value));
	EXPECT_FALSE(maths::IsNaN(not_infinity_d));
}

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

TEST(DecimalType, EpsilonValidation)
{
	maths::DecimalBits	last_exponent_bits = (((~((maths::DecimalBits(1) << DECIMAL_SHIFT) - 1)) << 1) >> 1);
	
	maths::Decimal		last_exponent = maths::DecimalBitsMapper(last_exponent_bits).value;//maths::BitsToDecimal(last_exponent_bits);
	maths::Decimal		next_value = maths::NextDecimalUp(last_exponent);
	maths::Decimal		computed_delta = next_value - last_exponent;

	maths::Decimal		next_plus_epsilon = next_value * (1._d + maths::machine_epsilon);
	maths::Decimal		next_minus_epsilon = next_value * (1._d - maths::machine_epsilon);
	maths::Decimal		theoretical_delta = next_plus_epsilon - next_minus_epsilon;

	EXPECT_EQ(computed_delta, theoretical_delta);
}

TEST(REDecimalTest, PrecisionVariable)
{
#if YS_REDECIMAL_HAS_PRECISE
	maths::REDecimal	A(3._d);
	maths::REDecimal	B(1.1_d);
	for (int i = 0; i < 10000; ++i)
	{
		A *= B;
		ASSERT_FALSE(maths::IsNaN(A.low_bound));
		ASSERT_FALSE(maths::IsNaN(A.high_bound));
		ASSERT_FALSE(maths::IsNaN(A.precise));
		ASSERT_TRUE(A.low_bound <= A.high_bound);
		ASSERT_TRUE(A.low_bound <= A.precise && A.high_bound >= A.precise);
	}
#endif
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

