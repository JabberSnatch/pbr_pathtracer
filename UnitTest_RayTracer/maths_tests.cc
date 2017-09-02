#include "gtest/gtest.h"

#include "global_definitions.h"
#include "maths/redecimal.h"

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

	EXPECT_TRUE(crappy_legacy::maths::IsInf(mapper_a.value));
	EXPECT_TRUE(crappy_legacy::maths::IsInf(mapper_b.value));
	EXPECT_FALSE(crappy_legacy::maths::IsInf(mapper_c.value));
	EXPECT_FALSE(crappy_legacy::maths::IsInf(not_infinity_d));
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

	EXPECT_FALSE(crappy_legacy::maths::IsNaN(mapper_a.value));
	EXPECT_FALSE(crappy_legacy::maths::IsNaN(mapper_b.value));
	EXPECT_TRUE(crappy_legacy::maths::IsNaN(mapper_c.value));
	EXPECT_FALSE(crappy_legacy::maths::IsNaN(not_infinity_d));
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

/*
TEST(DecimalType, Quadratic)
{
	maths::Decimal		A{ 5._d }, B{ 2._d }, C{ -3._d }, T0, T1;
	EXPECT_TRUE(maths::Quadratic(A, B, C, T0, T1));
	EXPECT_DECIMAL_EQ(T0, -1._d);
	EXPECT_DECIMAL_EQ(T1, 0.6_d);
}
*/

TEST(REDecimalTest, PrecisionVariable)
{
#if YS_REDECIMAL_HAS_PRECISE
	maths::REDecimal	A(3._d);
	maths::REDecimal	B(1.1_d);
	for (int i = 0; i < 10000; ++i)
	{
		A *= B;
		ASSERT_FALSE(std::isnan(A.low_bound));
		ASSERT_FALSE(std::isnan(A.high_bound));
		ASSERT_FALSE(std::isnan(A.precise));
		ASSERT_TRUE(A.low_bound <= A.high_bound);
		ASSERT_TRUE(A.low_bound <= A.precise && A.high_bound >= A.precise);
	}
#endif
}

/*
TEST(REDecimalTest, Quadratic)
{
	maths::Decimal		A{ 5._d }, B{ 2._d }, C{ -3._d }, T0, T1;
	maths::REDecimal	re_A{ A }, re_B{ B }, re_C{ C }, re_T0, re_T1;
	EXPECT_TRUE(maths::Quadratic(re_A, re_B, re_C, re_T0, re_T1));
	maths::Quadratic(A, B, C, T0, T1);
	EXPECT_DECIMAL_EQ(T0, re_T0.value);
	EXPECT_DECIMAL_EQ(T1, re_T1.value);
}
*/

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


TEST(SqrtTestFloat, Sqrt)
{
	maths::FloatBitsMapper mapper(0.f), root(0.f), check(0.f), sqr(0.f), sqrv(0.f);
	for (uint32_t i = 0; i <= maths::FloatMeta<float>::exponent_mask; i += 1000)
	{
		mapper.bits = i;
		root.value = crappy_legacy::maths::Sqrt(mapper.value);
		check.value = std::sqrt(mapper.value);
		sqr.value = root.value * root.value;
		sqrv.value = check.value * check.value;
		if (root.bits != check.bits)
		{
			EXPECT_LE(maths::Max(root.bits, check.bits) - maths::Min(root.bits, check.bits), 10u);
			EXPECT_LE(maths::Max(sqr.bits, sqrv.bits) - maths::Min(sqr.bits, sqrv.bits), 10u);
		}
	}
}

TEST(SqrtTestFloat, stdSqrtPerf)
{
	float storage = 0.f;
	maths::FloatBitsMapper mapper(0.f);
	for (uint32_t i = 0; i <= maths::FloatMeta<float>::exponent_mask; i += 1000)
	{
		mapper.bits = i;
		storage = std::sqrt(mapper.value);
	}
}

TEST(SqrtTestFloat, mathsSqrtPerf)
{
	float storage = 0.f;
	maths::FloatBitsMapper mapper(0.f);
	for (uint32_t i = 0; i <= maths::FloatMeta<float>::exponent_mask; i += 1000)
	{
		mapper.bits = i;
		storage = crappy_legacy::maths::Sqrt(mapper.value);
	}
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

