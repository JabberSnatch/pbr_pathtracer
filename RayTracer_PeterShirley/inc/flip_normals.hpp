#ifndef __YS_FLIP_NORMALS_HPP__
#define __YS_FLIP_NORMALS_HPP__

#include "hitable.hpp"


struct flip_normals : public hitable
{
	flip_normals() = delete;
	~flip_normals() = default;
	flip_normals(hitable& _rFlipped)
		:r_flipped{_rFlipped}
	{}

	bool hit(const ray& _r, float _tMin, float _tMax, hit_record& _rec) const override
	{
		if (r_flipped.hit(_r, _tMin, _tMax, _rec))
		{
			_rec.normal = -_rec.normal;
			return true;
		}
		else
			return false;

	}
	bool bounding_box(float _t0, float _t1, aabb& _box) const override
	{
		return r_flipped.bounding_box(_t0, _t1, _box);
	}
	virtual float pdf_value(const vec3& _origin, const vec3& _v) const override
	{
		return r_flipped.pdf_value(_origin, _v);
	}
	virtual vec3 random(const vec3& _origin) const override
	{
		return r_flipped.random(_origin);
	}

	hitable& r_flipped;
};



#endif // __YS_FLIP_NORMALS_HPP__
