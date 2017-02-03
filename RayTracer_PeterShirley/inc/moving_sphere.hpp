#ifndef __YS_MOVING_SPHERE_HPP__
#define __YS_MOVING_SPHERE_HPP__

#include "hitable.hpp"


struct moving_sphere : public hitable
{
	moving_sphere() = default;
	moving_sphere(const vec3& _cen0, const vec3& _cen1,
				  float _t0, float _t1, float _radius,
				  material* _pMaterial)
		:center0{_cen0}, center1{_cen1}, time0{_t0}, time1{_t1}, 
		 radius{_radius}, p_material{_pMaterial}
	{}

	vec3 center(float _time) const {
		return center0 + ((_time - time0) / (time1 - time0))*(center1 - center0);
	}

	bool hit(const ray& _r, float _tMin, float _tMax, hit_record& _record) const override;
	bool bounding_box(float _t0, float _t1, aabb& _box) const override;

	vec3 center0, center1;
	float time0, time1;
	float radius;
	std::shared_ptr<material> p_material;
};


bool
moving_sphere::hit(const ray& _r, float _tMin, float _tMax, hit_record& _record) const
{
	vec3 oc = _r.origin() - center(_r.time);
	float a = dot(_r.direction(), _r.direction());
	float b = dot(oc, _r.direction());
	float c = dot(oc, oc) - radius*radius;
	float discriminant = b*b - a*c;
	if (discriminant > 0)
	{
		float root = (-b - sqrt(discriminant)) / a;
		if (root < _tMax && root > _tMin)
		{
			_record.t = root;
			_record.p = _r.point_at_parameter(root);
			_record.normal = (_record.p - center(_r.time)) / radius;
			_record.p_material = p_material;
			return true;
		}
		root = (-b + sqrt(discriminant)) / a;
		if (root < _tMax && root > _tMin)
		{
			_record.t = root;
			_record.p = _r.point_at_parameter(root);
			_record.normal = (_record.p - center(_r.time)) / radius;
			_record.p_material = p_material;
			return true;
		}
	}
	return false;
}


bool
moving_sphere::bounding_box(float _t0, float _t1, aabb& _box) const
{
	aabb t0_box{center(_t0) - vec3::one() * abs(radius), 
				center(_t0) + vec3::one() * abs(radius)};
	aabb t1_box{center(_t1) - vec3::one() * abs(radius), 
				center(_t1) + vec3::one() * abs(radius)};
	_box = aabb{t0_box, t1_box};
	return true;
}


#endif // __YS_MOVING_SPHERE_HPP__
