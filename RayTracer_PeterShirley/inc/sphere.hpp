#ifndef __YS_SPHERE_HPP__
#define __YS_SPHERE_HPP__

#include <memory>
#include "hitable.hpp"

struct sphere : public hitable
{
	sphere() = default;
	sphere(vec3 _center, float _radius, material* _pMaterial)
		: center{_center}, radius{_radius}, p_material{_pMaterial}
	{}
	bool hit(const ray& _r, float _tMin, float _tMax, hit_record& _rec) const override;
	bool bounding_box(float _t0, float _t1, aabb& _box) const override;

	vec3 center;
	float radius;
	std::shared_ptr<material> p_material;
};


bool
sphere::hit(const ray& _r, float _tMin, float _tMax, hit_record& _rec) const
{
	vec3 oc = _r.origin() - center;
	float a = dot(_r.direction(), _r.direction());
	float b = dot(oc, _r.direction());
	float c = dot(oc, oc) - radius*radius;
	float discriminant = b*b - a*c;
	if (discriminant > 0)
	{
		float root = (-b - sqrt(b*b - a*c)) / a;
		if (root < _tMax && root > _tMin)
		{
			_rec.t = root;
			_rec.p = _r.point_at_parameter(_rec.t);
			_rec.normal = (_rec.p - center) / radius;
			_rec.p_material = p_material;
			return true;
		}

		root = (-b + sqrt(b*b - a*c)) / a;
		if (root < _tMax && root > _tMin)
		{
			_rec.t = root;
			_rec.p = _r.point_at_parameter(_rec.t);
			_rec.normal = (_rec.p - center) / radius;
			_rec.p_material = p_material;
			return true;
		}
	}
	return false;
}


bool
sphere::bounding_box(float _t0, float _t1, aabb& _box) const
{
	_box = aabb{center - vec3::one() * abs(radius), center + vec3::one() * abs(radius)};
	return true;
}


#endif // __YS_SPHERE_HPP__
