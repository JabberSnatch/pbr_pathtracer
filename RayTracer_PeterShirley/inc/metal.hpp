#ifndef __YS_METAL_HPP__
#define __YS_METAL_HPP__

#include "material.hpp"


struct metal : public material
{
	metal() = default;
	metal(const vec3& _albedo, float _fuzziness)
		:albedo{_albedo}, fuzziness{_fuzziness}
	{}

	bool scatter(const ray& _rayIn,
				 const hit_record& _hrecord,
				 scatter_record& _srecord) const override
	{
		vec3 reflected{reflect(unit_vector(_rayIn.direction()), _hrecord.normal)};
		_srecord.is_specular = true;
		_srecord.specular_ray = ray{
			_hrecord.p, 
			reflected + fuzziness*random::in_unit_sphere(), 
			_rayIn.time
		};
		_srecord.attenuation = albedo;
		_srecord.p_pdf = nullptr;
		//return (dot(_rayOut.direction(), _record.normal) > 0);
		return true;
	}


	vec3 albedo;
	float fuzziness;
};


#endif // __YS_METAL_HPP__
