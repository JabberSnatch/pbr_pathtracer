#include "gtest/gtest.h"

#include "maths/vector.h"
#include "global_definitions.h"


template <typename T>
class FloatVectorTest : public ::testing::Test
{
protected:
	FloatVectorTest()
		: scalar{ 3._d }
	{
		for (uint32_t i = 0; i < test_vector.size; ++i)
		{
			test_vector[i] = maths::Decimal(i + 1);
			vector[i] = maths::Decimal(i + 1) * 2;
		}
	}

	using			tested_type = T;
	T				test_vector;
	maths::Decimal	scalar;
	T				vector;
};

typedef ::testing::Types<maths::Vector<maths::Decimal, 2>, maths::Vec3f, maths::Vec4f, maths::Vector<maths::Decimal, 50>> vector_types;
TYPED_TEST_CASE(FloatVectorTest, vector_types);

TYPED_TEST(FloatVectorTest, ScalarMultiplication)
{
	test_vector *= scalar;
	for (uint32_t i = 1; i <= test_vector.size; ++i)
		EXPECT_DECIMAL_EQ(test_vector[i - 1], i * scalar);
}

TYPED_TEST(FloatVectorTest, ScalarDivision)
{
	test_vector /= scalar;
	for (uint32_t i = 1; i <= test_vector.size; ++i)
		EXPECT_DECIMAL_EQ(test_vector[i - 1], i / scalar);
}

TYPED_TEST(FloatVectorTest, VectorMultiplication)
{
	test_vector *= vector;
	for (uint32_t i = 1; i <= test_vector.size; ++i)
		EXPECT_DECIMAL_EQ(test_vector[i - 1], 2 * i * i);
}

TYPED_TEST(FloatVectorTest, VectorDivision)
{
	test_vector /= vector;
	for (uint32_t i = 1; i <= test_vector.size; ++i)
		EXPECT_DECIMAL_EQ(test_vector[i - 1], 0.5_d);
}

TYPED_TEST(FloatVectorTest, VectorMin)
{
	tested_type min = maths::Min(test_vector, vector);
	for (uint32_t i = 0; i < min.size; ++i)
		EXPECT_DECIMAL_EQ(min[i], test_vector[i]);
}

TYPED_TEST(FloatVectorTest, VectorMax)
{
	auto max = maths::Max(test_vector, vector);
	for (uint32_t i = 0; i < max.size; ++i)
		EXPECT_DECIMAL_EQ(max[i], vector[i]);
}

TYPED_TEST(FloatVectorTest, EqualOp)
{
	EXPECT_TRUE(test_vector == test_vector);
	EXPECT_FALSE(test_vector != test_vector);
	EXPECT_FALSE(test_vector == vector);
	EXPECT_TRUE(test_vector != vector);
}

TYPED_TEST(FloatVectorTest, MinMaxDimension)
{
	EXPECT_EQ(maths::MaximumDimension(vector), vector.size - 1u);
	EXPECT_EQ(maths::MinimumDimension(vector), 0u);
}


class Vec3fTest : public ::testing::Test
{
protected:
	Vec3fTest() :
		vector{ 1._d, 2._d, 3._d },
		x{ 1._d, 0._d, 0._d },
		y{ 0._d, 1._d, 0._d },
		z{ 0._d, 0._d, 1._d }
	{}

	maths::Vec3f	vector;
	maths::Vec3f	x;
	maths::Vec3f	y;
	maths::Vec3f	z;
};

TEST_F(Vec3fTest, DotProduct)
{
	EXPECT_DECIMAL_EQ(maths::Dot(x, y), 0._d);
	EXPECT_DECIMAL_EQ(maths::Dot(x, x), 1._d);
	EXPECT_TRUE(maths::Dot(vector, x) > 0._d);
}

TEST_F(Vec3fTest, CrossProduct)
{
	EXPECT_TRUE(maths::Cross(x, y) == z);
	EXPECT_DECIMAL_EQ(maths::SqrLength(maths::Cross(x, y)), 1._d);

	maths::Vec3f	cross{ maths::Cross(vector, x) };
	EXPECT_DECIMAL_EQ(maths::Dot(cross, vector), 0._d);
	EXPECT_DECIMAL_EQ(maths::Dot(cross, x), 0._d);

	EXPECT_DECIMAL_EQ(maths::Dot(maths::Cross(y, x), z), -1._d);
}

TEST_F(Vec3fTest, Reflect)
{
	maths::Vec3f	direction = maths::Normalized(vector);
	maths::Vec3f	reflected = maths::Reflect(direction, x);

	EXPECT_DECIMAL_EQ(maths::Dot(direction, x), maths::Dot(reflected, x));
	EXPECT_DECIMAL_EQ(maths::Dot(direction, y), -maths::Dot(reflected, y));
	EXPECT_DECIMAL_EQ(maths::Dot(direction, z), -maths::Dot(reflected, z));
}



typedef ::testing::Types<maths::Norm3f> normal_types;

template <typename T>
class NormalTest : public ::testing::Test
{
protected:
	NormalTest() :
		x(0._d), y(0._d), z(0._d)
	{
		for (uint32_t i = 1; i <= normalA.size; ++i)
		{
			normalA[i - 1] = maths::Decimal(i);
			normalB[i - 1] = maths::Decimal(i) * 2._d;
		}
		x[0] = 1._d;
		y[1] = 1._d;
		z[2] = 1._d;
	}

	T		normalA;
	T		normalB;
	T		x;
	T		y;
	T		z;
};

TYPED_TEST_CASE(NormalTest, normal_types);

TYPED_TEST(NormalTest, Normalization)
{
	EXPECT_GT(maths::Length(normalA), 1._d);
	auto	normalized = maths::Normalized(normalA);
	EXPECT_DECIMAL_EQ(maths::Length(normalized), 1._d);
}

TYPED_TEST(NormalTest, Directions)
{
	auto	x_facing = maths::FaceForward(normalA, x);
	auto	normalized = maths::Normalized(normalA);
	auto	z_facing = maths::FaceForward(normalized, z);
	auto	minusZ_facing = maths::FaceForward(normalized, -z);

	EXPECT_GT(maths::Dot(x_facing, x), 0._d);
	EXPECT_GT(maths::Dot(x_facing, normalA), 0._d);

	EXPECT_DECIMAL_EQ(maths::Dot(z_facing, minusZ_facing), -1._d);
	EXPECT_GT(maths::Dot(z_facing, z), 0._d);
	EXPECT_GT(maths::Dot(z_facing, normalized), 0._d);
	EXPECT_LT(maths::Dot(minusZ_facing, normalized), 0._d);
}
