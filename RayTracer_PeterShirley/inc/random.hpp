#ifndef __YS_RANDOM_HPP__
#define __YS_RANDOM_HPP__

#include <random>

#include "vec3.hpp"


struct random
{
	random() = delete;
	~random() = delete;

	static inline float sample() {
		return zero_one(random_generator);
	}

	static inline float sample_relative() {
		return minus_one_one(random_generator);
	}

	static inline vec3 ninja() {
		return vec3{sample(), sample(), sample()};
	}

	static vec3 in_unit_sphere() {
		vec3 direction{ vec3::one() };

		while (direction.squared_length() >= 1.f)
			direction = 2.f * ninja() - vec3::one();

		return direction;
	}

	static vec3 in_unit_disk()
	{
		vec3 point{ vec3::one() };
		while (dot(point, point) >= 1.0f)
		{
			point = 2.f * vec3{ sample(), sample(), .0f } - vec3{1.f, 1.f, .0f};
		}
		return point;
	}

	static std::random_device rd;
	static std::mt19937 random_generator;
	static std::uniform_real_distribution<float> zero_one;
	static std::uniform_real_distribution<float> minus_one_one;
};


std::random_device random::rd{};
std::mt19937 random::random_generator{random::rd()};
std::uniform_real_distribution<float> random::zero_one{0.f, 1.f};
std::uniform_real_distribution<float> random::minus_one_one{-1.f, 1.f};



#endif // __YS_RANDOM_HPP__
