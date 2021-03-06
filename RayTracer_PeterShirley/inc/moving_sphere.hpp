#ifndef __YS_MOVING_SPHERE_HPP__
#define __YS_MOVING_SPHERE_HPP__

#include "hitable.hpp"


struct moving_sphere : public hitable
{
	moving_sphere() = delete;
	~moving_sphere() = default;
	moving_sphere(const vec3& _cen0, const vec3& _cen1,
				  float _t0, float _t1, float _radius,
				  material& _rMaterial)
		:center0{_cen0}, center1{_cen1}, time0{_t0}, time1{_t1}, 
		 radius{_radius}, r_material{_rMaterial}
	{}

	vec3 center(float _time) const {
		return center0 + ((_time - time0) / (time1 - time0))*(center1 - center0);
	}

	bool hit(const ray& _r, float _tMin, float _tMax, hit_record& _record) const override;
	bool bounding_box(float _t0, float _t1, aabb& _box) const override;

	void get_uv(const vec3& _dir, float& _u, float& _v) const
	{
		float phi = atan2(_dir.z(), _dir.x());
		float theta = asin(-_dir.y());
		constexpr float pi = 3.1415926535f;
		_u = 1.f - (phi + pi) / (2.f * pi);
		_v = (theta + pi / 2.f) / pi;
	}

	vec3 center0, center1;
	float time0, time1;
	float radius;
	material& r_material;
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
			_record.p_material = &r_material;
			get_uv(_record.normal, _record.u, _record.v);
			return true;
		}
		root = (-b + sqrt(discriminant)) / a;
		if (root < _tMax && root > _tMin)
		{
			_record.t = root;
			_record.p = _r.point_at_parameter(root);
			_record.normal = (_record.p - center(_r.time)) / radius;
			_record.p_material = &r_material;
			get_uv(_record.normal, _record.u, _record.v);
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
