#ifndef __YS_MULTIPLY_HPP__
#define __YS_MULTIPLY_HPP__

#include "material.hpp"
#include "orthonormal_basis.hpp"


struct multiply : public material
{
	multiply(float _scaleFactor)
		:scale_factor{_scaleFactor}
	{}

	bool scatter(const ray& _rayIn,
				 const hit_record& _hrecord,
				 scatter_record& _srecord) const override
	{
		_srecord.is_specular = true;
		_srecord.attenuation = vec3::one() * scale_factor;
		_srecord.p_pdf = std::unique_ptr<pdf>{new cosine_pdf{_hrecord.normal}};

		onb uvw{_hrecord.normal};
		vec3 direction = uvw.local(g_RNG.cosine_direction());
		_srecord.specular_ray = ray{_hrecord.p, 
									//reflect(unit_vector(_rayIn.direction()), _hrecord.normal), 
									direction,
									_rayIn.time};
		return true;
	}

	float scattering_pdf(const ray& _rayIn, const hit_record& _record, const ray& _scattered) const override
	{
		float cosine = dot(_record.normal, unit_vector(_scattered.direction()));
		cosine = ffmax(cosine, 0.f);
		return cosine / 3.1415926535f;
	}

	float scale_factor;
};



#endif // __YS_MULTIPLY_HPP__
