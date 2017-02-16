#ifndef __YS_DIFFUSE_LIGHT_HPP__
#define __YS_DIFFUSE_LIGHT_HPP__

#include "material.hpp"
#include "texture.hpp"


struct diffuse_light : public material
{
	diffuse_light() = delete;
	~diffuse_light() = default;
	diffuse_light(texture& _emission)
		:emission{_emission}
	{}

	bool scatter(const ray& _rayIn,
				 const hit_record& _hrecord,
				 scatter_record& _srecord) const override
	{
		return false;
	}

	vec3 emit(const ray& _rayIn, 
			  const hit_record& _rec, 
			  float _u, float _v, const vec3& _p) const override
	{
		if (dot(_rec.normal, _rayIn.direction()) < .0f)
			return emission.value(_u, _v, _p);
		else
			return vec3::zero();
	}

	texture& emission;
};


#endif // __YS_DIFFUSE_LIGHT_HPP__
