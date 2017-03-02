#ifndef __YS_TRIANGLE_HPP__
#define __YS_TRIANGLE_HPP__

#include "hitable.hpp"

//#define BACKFACE_CULLING
#define DOUBLE_FACED_TRIANGLES


struct triangle : public hitable
{
	triangle(vec3 const &_v1, vec3 const &_v2, vec3 const &_v3, material& _rMaterial)
		:v1 {_v1}, v2 {_v2}, v3 {_v3}, r_material {_rMaterial}
	{}

	bool hit(const ray& _r, float _tMin, float _tMax, hit_record& _rec) const
	{
		vec3 e1 {v2 - v1}, e2 {v3 - v1};
		vec3 normal {cross(e1, e2)};
#ifdef BACKFACE_CULLING
		if (dot(normal, _r.direction()) > 0.f)
			return false;
#endif
#ifdef DOUBLE_FACED_TRIANGLES
		if (dot(normal, _r.direction()) > 0.f)
			normal = -normal;
#endif // DOUBLE_FACED_TRIANGLES

		vec3 P {cross(_r.direction(), e2)};
		float det {dot(e1, P)};

		if (det > -0.0001f && det < 0.0001f)
			return false;

		float inv_det {1.f / det};
		vec3 T {_r.origin() - v1};

		float u {dot(T, P) * inv_det};
		if (u < 0.f || u > 1.f)
			return false;

		vec3 Q {cross(T, e1)};
		float v {dot(_r.direction(), Q) * inv_det};
		if (v < 0.f || u + v > 1.f)
			return false;

		float t {dot(e2, Q) * inv_det};

		if (t > _tMin && t < _tMax)
		{
			_rec.normal = normal;
			_rec.p = _r.point_at_parameter(t);
			_rec.p_material = &r_material;
			_rec.t = t;
			_rec.u = u;
			_rec.v = v;
			return true;
		}

		return false;
	}
	bool bounding_box(float _t0, float _t1, aabb& _box) const
	{
		vec3 min {vec3::one() * FLT_MAX};
		vec3 max {vec3::one() * -FLT_MAX};
		
		for (int i {0}; i < 3; ++i)
		{
			min[i] = ffmin(v1[i]/* - .0001f*/, min[i]);
			min[i] = ffmin(v2[i]/* - .0001f*/, min[i]);
			min[i] = ffmin(v3[i]/* - .0001f*/, min[i]);

			max[i] = ffmax(v1[i]/* + .0001f*/, max[i]);
			max[i] = ffmax(v2[i]/* + .0001f*/, max[i]);
			max[i] = ffmax(v3[i]/* + .0001f*/, max[i]);
		}

		_box = aabb {min, max};
		return true;
	}

	vec3		v1, v2, v3;
	material&	r_material;
};


#endif // __YS_TRIANGLE_HPP__
