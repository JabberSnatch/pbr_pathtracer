#ifndef __YS_LAMBERTIAN_HPP__
#define __YS_LAMBERTIAN_HPP__

#include "material.hpp"
#include "random.hpp"
#include "texture.hpp"


struct lambertian : public material
{
	lambertian() = default;
	lambertian(const vec3& _albedo)
		:albedo(_albedo)
	{}

	bool scatter(const ray& _rayIn,
				 const hit_record& _record,
				 vec3& _attenuation,
				 ray& _rayOut) const override
	{
		vec3 target{_record.p + _record.normal + random::in_unit_sphere()};
		_rayOut = ray{_record.p, target - _record.p, _rayIn.time};
		_attenuation = albedo;
		return true;
	}

	vec3 albedo;
};



#endif // __YS_LAMBERTIAN_HPP__
