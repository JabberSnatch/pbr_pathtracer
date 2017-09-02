#include "gtest/gtest.h"

#include "core/rng.h"
#include "pcg_random.hpp"
#include "pcg_extras.hpp"

constexpr uint64_t seed{ 12439587162u };
constexpr uint32_t trunc_seed{ static_cast<uint32_t>(seed) };

TEST(RandomNumberGeneration, ConformanceTest)
{
	pcg32_k2	ground_truth{ seed };
	core::RNG	subject{ seed };
	EXPECT_EQ(subject(), ground_truth());
	for (int i = 0; i < 10000; ++i)
	{
		ground_truth();
		subject();
	}
	EXPECT_EQ(subject(), ground_truth());
}

TEST(RandomNumberGeneration, InvXorshift)
{
	const uint32_t	ground_truth{ pcg_extras::unxorshift(trunc_seed, 32u, 10u) };
	const uint32_t	subject{ core::RNG::inv_xorshift(trunc_seed, 10u) };
	EXPECT_EQ(subject, ground_truth);
}

TEST(RandomNumberGeneration, RotateRight)
{
	const uint32_t	ground_truth{ pcg_extras::rotr(trunc_seed, 10u) };
	const uint32_t	subject{ core::RNG::rotate_right(trunc_seed, 10u) };
	EXPECT_EQ(subject, ground_truth);
}
