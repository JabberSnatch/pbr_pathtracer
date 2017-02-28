#ifndef __YS_LAMBERTIAN_HPP__
#define __YS_LAMBERTIAN_HPP__

#include "material.hpp"
#include "random.hpp"
#include "texture.hpp"
#include "orthonormal_basis.hpp"
#include "cosine_pdf.hpp"


struct lambertian : public material
{
	lambertian() = delete;
	~lambertian() = default;
	lambertian(const texture& _albedo)
		:albedo{_albedo}
	{}

	bool scatter(const ray& _rayIn,
				 const hit_record& _hrecord,
				 scatter_record& _srecord) const override
	{
		_srecord.is_specular = false;
		_srecord.attenuation = albedo.value(_hrecord.u, _hrecord.v, _hrecord.p);
		_srecord.p_pdf = std::unique_ptr<pdf>{new cosine_pdf{_hrecord.normal}};
		
		onb uvw{_hrecord.normal};
		vec3 direction = uvw.local(g_RNG.cosine_direction());
		_srecord.specular_ray = ray{_hrecord.p, unit_vector(direction), _rayIn.time};
		return true;

		//_rayOut = ray{_record.p, unit_vector(direction), _rayIn.time};
		//_attenuation = albedo.value(_record.u, _record.v, _record.p);
		//_pdf = dot(uvw.w(), _rayOut.direction()) / 3.1415926535f;
		//return true;
	}

	float scattering_pdf(const ray& _rayIn, const hit_record& _record, const ray& _scattered) const override
	{
		float cosine = dot(_record.normal, unit_vector(_scattered.direction()));
		cosine = ffmax(cosine, 0.f);
		return cosine / 3.1415926535f;
	}

	const texture& albedo;
};



#endif // __YS_LAMBERTIAN_HPP__
