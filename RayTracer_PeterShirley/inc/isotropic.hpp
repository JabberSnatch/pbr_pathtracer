#ifndef __YS_ISOTROPIC_HPP__
#define __YS_ISOTROPIC_HPP__

#include "material.hpp"
#include "texture.hpp"
#include "random.hpp"


struct isotropic : public material
{
	isotropic() = delete;
	~isotropic() = default;
	isotropic(texture& _albedo)
		:albedo{_albedo}
	{}

	bool scatter(const ray& _rayIn,
				 const hit_record& _hrecord,
				 scatter_record& _srecord) const override
	{
		_srecord.specular_ray = ray{_hrecord.p, random::in_unit_sphere()};
		_srecord.attenuation = albedo.value(_hrecord.u, _hrecord.v, _hrecord.p);
		_srecord.is_specular = true;
		_srecord.p_pdf = nullptr;
		return true;
	}

	texture& albedo;
};


#endif // __YS_ISOTROPIC_HPP__
