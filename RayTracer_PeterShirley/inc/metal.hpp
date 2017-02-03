#ifndef __YS_METAL_HPP__
#define __YS_METAL_HPP__

#include "material.hpp"


struct metal : public material
{
	metal() = delete;
	metal(const vec3& _albedo, float _fuzziness)
		:albedo{_albedo}, fuzziness{_fuzziness}
	{}

	bool scatter(const ray& _rayIn,
				 const hit_record& _record,
				 vec3& _attenuation,
				 ray& _rayOut) const override
	{
		vec3 reflected{reflect(unit_vector(_rayIn.direction()), _record.normal)};
		//_rayOut = ray{_record.p, -_record.normal};
		//while (dot(_rayOut.direction(), _record.normal) < 0)
		_rayOut = ray{_record.p, reflected + fuzziness*random::in_unit_sphere(), _rayIn.time};
		_attenuation = albedo;
		//return true;
		return (dot(_rayOut.direction(), _record.normal) > 0);
	}


	vec3 albedo;
	float fuzziness;
};


#endif // __YS_METAL_HPP__
