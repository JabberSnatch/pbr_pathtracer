#ifndef __YS_ORTHONORMAL_BASIS_HPP__
#define __YS_ORTHONORMAL_BASIS_HPP__

#include "vec3.hpp"


struct onb
{
	onb() = default;
	~onb() = default;
	onb(const vec3& _forward);

	inline vec3 operator[](int i) const { return axis[i]; }
	vec3 u() const			{ return axis[0]; }
	vec3 v() const			{ return axis[1]; }
	vec3 w() const			{ return axis[2]; }
	vec3 local(float a, float b, float c) const { return a*u() + b*v() + c*w(); }
	vec3 local(const vec3& _v) const { return _v.x()*u() + _v.y()*v() + _v.z()*w(); }

	vec3 axis[3];
};


onb::onb(const vec3& _forward)
{
	axis[2] = unit_vector(_forward);
	vec3 a;
	if (fabsf(w().x()) > 0.9f)
		a = vec3{0.f, 1.f, 0.f};
	else
		a = vec3{1.f, 0.f, 0.f};
	axis[1] = unit_vector(cross(w(), a));
	axis[0] = cross(w(), v());
}


#endif // __YS_ORTHONORMAL_BASIS_HPP__
