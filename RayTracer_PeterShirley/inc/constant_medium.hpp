#ifndef __YS_CONSTANT_MEDIUM_HPP__
#define __YS_CONSTANT_MEDIUM_HPP__

#include "hitable.hpp"
#include "texture.hpp"
#include "isotropic.hpp"
#include "random.hpp"


struct constant_medium : public hitable
{
	constant_medium() = delete;
	~constant_medium() = default;
	constant_medium(hitable& _boundary, float _density, texture& _albedo)
		:boundary{_boundary}, density{_density}, phase_function{_albedo}
	{}

	bool hit(const ray& _r, float _tMin, float _tMax, hit_record& _rec) const override;
	bool bounding_box(float _t0, float _t1, aabb& _box) const override
	{
		return boundary.bounding_box(_t0, _t1, _box);
	}

	hitable& boundary;
	float density;
	isotropic phase_function;
};


bool
constant_medium::hit(const ray& _r, float _tMin, float _tMax, hit_record& _rec) const
{
	hit_record rec1, rec2;
	if (boundary.hit(_r, -FLT_MAX, FLT_MAX, rec1))
	{
		if (boundary.hit(_r, rec1.t + .00001f, FLT_MAX, rec2))
		{
			if (rec1.t < _tMin)
				rec1.t = _tMin;
			if (rec2.t < _tMax)
				rec2.t = _tMax;
			if (rec1.t >= rec2.t)
				return false;
			if (rec1.t < 0)
				rec1.t = 0;
			float distance_inside_boundary = (rec2.t - rec1.t)*_r.direction().length();
			float hit_distance = -(1.f / density)*log(g_RNG.sample());
			if (hit_distance < distance_inside_boundary)
			{
				_rec.t = rec1.t + hit_distance / _r.direction().length();
				_rec.p = _r.point_at_parameter(_rec.t);
				_rec.normal = vec3{1.f, 0.f, 0.f};
				_rec.p_material = &phase_function;
				return true;
			}
		}
	}
	return false;
}


#endif // __YS_CONSTANT_MEDIUM_HPP__
