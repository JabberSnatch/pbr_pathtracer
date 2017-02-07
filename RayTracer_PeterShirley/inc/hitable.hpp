#ifndef __YS_HITABLE_HPP__
#define __YS_HITABLE_HPP__

#include <memory>
#include "ray.hpp"
#include "aabb.hpp"

struct material;


struct hit_record
{
	float t;
	vec3 p;
	vec3 normal;
	material* p_material;
};


struct hitable
{
	virtual ~hitable() = default;
	virtual bool hit(const ray& _r, float _tMin, float _tMax, hit_record& _rec) const = 0;
	virtual bool bounding_box(float _t0, float _t1, aabb& _box) const = 0;
};


#endif // __YS_HITABLE_HPP__
