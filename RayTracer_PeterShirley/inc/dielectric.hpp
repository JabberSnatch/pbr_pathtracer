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
				 const hit_record& _hrecord,
				 scatter_record& _srecord) const override
	{
		vec3 outward_normal, refracted;
		vec3 reflected{reflect(_rayIn.direction(), _hrecord.normal)};
		float reflect_probability;
		float cosine;
		float ni_over_nt;
		_srecord.attenuation = vec3(1.f, 1.f, 1.f);
		
		if (dot(_rayIn.direction(), _hrecord.normal) > 0)
		{
			outward_normal = -_hrecord.normal;
			ni_over_nt = refraction_index;
			cosine = refraction_index * dot(_rayIn.direction(), _hrecord.normal) / _rayIn.direction().length();
		}
		else
		{
			outward_normal = _hrecord.normal;
			ni_over_nt = 1.f / refraction_index;
			cosine = -dot(_rayIn.direction(), _hrecord.normal) / _rayIn.direction().length();
		}

		if (refract(_rayIn.direction(), outward_normal, ni_over_nt, refracted))
			reflect_probability = schlick(cosine);
		else
			reflect_probability = 1.0f;

		if (random::sample() < reflect_probability)
			_srecord.specular_ray = ray{_hrecord.p, reflected, _rayIn.time};
		else
			_srecord.specular_ray = ray{_hrecord.p, refracted, _rayIn.time};

		_srecord.is_specular = true;
		_srecord.p_pdf = nullptr;

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
