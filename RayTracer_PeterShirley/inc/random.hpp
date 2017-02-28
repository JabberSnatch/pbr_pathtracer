#ifndef __YS_RANDOM_HPP__
#define __YS_RANDOM_HPP__

#include <random>
#include <mutex>

#include "vec3.hpp"


struct random
{
	inline float sample() {
		return zero_not_one(random_generator);
	}

	inline float sample_one_included() {
		return zero_one(random_generator);
	}

	inline float sample_relative() {
		return minus_one_one(random_generator);
	}

	inline vec3 ninja() {
		return vec3{sample(), sample(), sample()};
	}

	vec3 in_unit_sphere() {
		vec3 direction{ vec3::one() };

		while (direction.squared_length() >= 1.f)
			direction = 2.f * ninja() - vec3::one();

		return direction;
	}

	vec3 on_unit_sphere()
	{
		float phi = 2.f * 3.1415926545f * sample();
		float cos_theta = 1.f - 2.f * sample();
		float x = cos(phi) * sqrt(1.f - cos_theta*cos_theta);
		float y = sin(phi) * sqrt(1.f - cos_theta*cos_theta);
		float z = cos_theta;

		return vec3{x, y, z};
	}

	vec3 cosine_direction()
	{
		float r1 = sample();
		float r2 = sample();
		float z = sqrt(1 - r2);
		float phi = 2.f * 3.1415926535f * r1;
		float x = cos(phi)*2.f*sqrt(r2);
		float y = sin(phi)*2.f*sqrt(r2);
		return vec3{x, y, z};
	}

	vec3 in_unit_disk()
	{
		vec3 point{ vec3::one() };
		while (dot(point, point) >= 1.0f)
		{
			point = 2.f * vec3{ sample(), sample(), .0f } - vec3{1.f, 1.f, .0f};
		}
		return point;
	}

	inline vec3 to_sphere(float _radius, float _distanceSquared)
	{
		float r1 = sample();
		float r2 = sample();
		float z = 1.f + r2*(sqrt(1.f - _radius*_radius / _distanceSquared) - 1.f);
		float phi = 2.f * 3.1415926535f * r1;
		float x = cos(phi) * sqrt(1.f - z*z);
		float y = sin(phi) * sqrt(1.f - z*z);
		return vec3{x, y, z};
	}


	std::random_device rd{};
	std::mt19937 random_generator{rd()};
	std::uniform_real_distribution<float> zero_one{0.f, 1.f};
	std::uniform_real_distribution<float> zero_not_one{0.f, one_minus_epsilon};
	std::uniform_real_distribution<float> minus_one_one{-1.f, 1.f};

	static const float one_minus_epsilon;
};

static thread_local random g_RNG{};


const float			random::one_minus_epsilon = 1.f - FLT_MIN; // 0x1.fffffep-1; c++17 :(
//std::random_device						random::rd{};
//std::mt19937							random::random_generator{random::rd()};
//std::uniform_real_distribution<float>	random::zero_one{0.f, 1.f};
//std::uniform_real_distribution<float>	random::minus_one_one{-1.f, 1.f};



#endif // __YS_RANDOM_HPP__
