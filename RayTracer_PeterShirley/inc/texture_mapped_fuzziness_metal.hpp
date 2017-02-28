#ifndef __YS_TEXTURE_MAPPED_FUZZINESS_METAL_HPP__
#define __YS_TEXTURE_MAPPED_FUZZINESS_METAL_HPP__

#include "material.hpp"
#include "texture.hpp"
#include "random.hpp"


struct texture_mapped_fuzziness_metal : public material
{
	texture_mapped_fuzziness_metal() = delete;
	~texture_mapped_fuzziness_metal() = default;
	texture_mapped_fuzziness_metal(const vec3& _albedo, texture& _map)
		:albedo{_albedo}, fuzziness_map{_map}
	{}

	bool scatter(const ray& _rayIn,
				 const hit_record& _hrecord,
				 scatter_record& _srecord) const override
	{
		vec3 reflected{reflect(unit_vector(_rayIn.direction()), _hrecord.normal)};
		float fuzziness = 1.f - fuzziness_map.value(_hrecord.u, _hrecord.v, _hrecord.p).x();
		_srecord.is_specular = true;
		_srecord.p_pdf = nullptr;
		_srecord.specular_ray = ray{
			_hrecord.p, 
			reflected + fuzziness*g_RNG.in_unit_sphere(), 
			_rayIn.time
		};
		_srecord.attenuation = albedo;

		//return (dot(_rayOut.direction(), _record.normal) > 0);
		return true;
	}


	vec3 albedo;
	texture& fuzziness_map;
};


#endif // __YS_TEXTURE_MAPPED_FUZZINESS_METAL_HPP__
