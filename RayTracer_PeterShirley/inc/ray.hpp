#ifndef __YS_RAY_HPP__
#define __YS_RAY_HPP__

#include "vec3.hpp"


struct ray
{
	ray() = default;
	ray(const vec3& a, const vec3& b, float _time = .0f)
		:A{a}, B{b}, time{_time}
	{}

	vec3 origin() const { return A; }
	vec3 direction() const { return B; }
	vec3 point_at_parameter(float _t) const { return A + _t * B; }

	vec3 A;
	vec3 B;
	float time;
};



#endif // __YS_RAY_HPP__
