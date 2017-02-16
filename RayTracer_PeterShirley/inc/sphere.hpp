#ifndef __YS_SPHERE_HPP__
#define __YS_SPHERE_HPP__

#include <memory>
#include "hitable.hpp"
#include "random.hpp"
#include "orthonormal_basis.hpp"

struct sphere : public hitable
{
	sphere() = default;
	sphere(vec3 _center, float _radius, material& _rMaterial)
		: center{_center}, radius{_radius}, r_material{_rMaterial}
	{}
	bool hit(const ray& _r, float _tMin, float _tMax, hit_record& _rec) const override;
	bool bounding_box(float _t0, float _t1, aabb& _box) const override;
	float pdf_value(const vec3& _origin, const vec3& _v) const override;
	vec3 random(const vec3& _origin) const override;

	void get_uv(const vec3& _dir, float& _u, float& _v) const
	{
		float phi = atan2(_dir.z(), _dir.x());
		float theta = asin(-_dir.y());
		constexpr float pi = 3.1415926535f;
		_u = 1.f - (phi + pi) / (2.f * pi);
		_v = (theta + pi / 2.f) / pi;
	}

	vec3 center;
	float radius;
	material& r_material;
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
			_rec.p_material = &r_material;
			get_uv(_rec.normal, _rec.u, _rec.v);
			return true;
		}

		root = (-b + sqrt(b*b - a*c)) / a;
		if (root < _tMax && root > _tMin)
		{
			_rec.t = root;
			_rec.p = _r.point_at_parameter(_rec.t);
			_rec.normal = (_rec.p - center) / radius;
			_rec.p_material = &r_material;
			get_uv(_rec.normal, _rec.u, _rec.v);
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


float
sphere::pdf_value(const vec3& _origin, const vec3& _v) const
{
	hit_record rec;
	if (hit(ray{_origin, _v}, 0.001f, FLT_MAX, rec))
	{
		float cos_theta_max = sqrt(1.f - radius*radius / (center - _origin).squared_length());
		float solid_angle = 2.f*3.1415926535f * (1.f - cos_theta_max);
		return 1.f / solid_angle;
	}
	else
		return 0.f;
}


vec3
sphere::random(const vec3& _origin) const
{
	vec3 direction = center - _origin;
	float distance_squared = direction.squared_length();
	onb uvw{direction};
	return uvw.local(random::to_sphere(radius, distance_squared));
}


#endif // __YS_SPHERE_HPP__
