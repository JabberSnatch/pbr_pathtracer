#ifndef __YS_MATERIAL_HPP__
#define __YS_MATERIAL_HPP__

#include "hitable.hpp"
#include "pdf.hpp"
#include "random.hpp"


struct scatter_record
{
	ray specular_ray;
	bool is_specular;
	vec3 attenuation;
	std::unique_ptr<pdf> p_pdf;
};


struct material
{
	virtual ~material() = default;

	virtual bool scatter(const ray& _rayIn, 
						 const hit_record& _hrecord, 
						 scatter_record& _srecord) const
	{ return false; }

	virtual float scattering_pdf(const ray& _rayIn, 
								 const hit_record& _rec, 
								 const ray& _scattered) const
	{ return 0.0f; }

	virtual vec3 emit(const ray& _rayIn, 
					  const hit_record& _rec, 
					  float _u, float _v, const vec3& _p) const 
	{ return vec3::zero(); }
};


#endif // __YS_MATERIAL_HPP__
