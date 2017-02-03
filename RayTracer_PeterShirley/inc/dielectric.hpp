#ifndef __YS_DIELECTRIC_HPP__
#define __YS_DIELECTRIC_HPP__

#include "material.hpp"


struct dielectric : public material
{
	dielectric() = delete;
	dielectric(float _refractionIndex)
		:refraction_index{_refractionIndex}
	{}

	bool scatter(const ray& _rayIn, 
				 const hit_record& _record, 
				 vec3& _attenuation, 
				 ray& _rayOut) const override
	{
		vec3 outward_normal, refracted;
		vec3 reflected{reflect(_rayIn.direction(), _record.normal)};
		float reflect_probability;
		float cosine;
		float ni_over_nt;
		_attenuation = vec3(1.f, 1.f, 1.f);
		
		if (dot(_rayIn.direction(), _record.normal) > 0)
		{
			outward_normal = -_record.normal;
			ni_over_nt = refraction_index;
			cosine = refraction_index * dot(_rayIn.direction(), _record.normal) / _rayIn.direction().length();
		}
		else
		{
			outward_normal = _record.normal;
			ni_over_nt = 1.f / refraction_index;
			cosine = -dot(_rayIn.direction(), _record.normal) / _rayIn.direction().length();
		}

		if (refract(_rayIn.direction(), outward_normal, ni_over_nt, refracted))
			reflect_probability = schlick(cosine);
		else
			reflect_probability = 1.0f;

		if (random::sample() < reflect_probability)
			_rayOut = ray{_record.p, reflected, _rayIn.time};
		else
			_rayOut = ray{_record.p, refracted, _rayIn.time};

		return true;
	}

	float schlick(float _cosine) const
	{
		float r0 = (1.f - refraction_index) / (1.f + refraction_index);
		r0 = r0*r0;
		return r0 + (1.f-r0)*pow(1 - _cosine, 5);
	}


	float refraction_index;
};



#endif // __YS_DIELECTRIC_HPP__
