#ifndef __YS_AA_RECTANGLES_HPP__
#define __YS_AA_RECTANGLES_HPP__

#include "hitable.hpp"
#include "material.hpp"


struct xy_rect : public hitable
{
	xy_rect() = delete;
	~xy_rect() = default;
	xy_rect(float _x0, float _x1, float _y0, float _y1, float _k, material& _rMaterial)
		:x0{_x0}, x1{_x1}, y0{_y0}, y1{_y1}, k{_k}, r_material{_rMaterial}
	{}

	bool hit(const ray& _r, float _tMin, float _tMax, hit_record& _rec) const
	{
		float t = (k - _r.origin().z()) / _r.direction().z();
		if (!(t == t))
		{
			//std::cout << "NaN found in xy_rect::hit" << std::endl;
			return false;
		}
		if (t < _tMin || t > _tMax)
			return false;
		float x = _r.origin().x() + t*_r.direction().x();
		float y = _r.origin().y() + t*_r.direction().y();
		if (x < x0 || x > x1 || y < y0 || y > y1)
			return false;

		_rec.u = (x - x0) / (x1 - x0);
		_rec.v = (y - y0) / (y1 - y1);
		_rec.t = t;
		_rec.p_material = &r_material;
		_rec.p = _r.point_at_parameter(t);
		_rec.normal = vec3{0.f, 0.f, 1.f};
		return true;
	}
	bool bounding_box(float _t0, float _t1, aabb& _box) const
	{
		_box = aabb{vec3{x0, y0, k-.0001f}, vec3{x1, y1, k+.0001f}};
		return true;
	}

	float x0, x1, y0, y1, k;
	material& r_material;
};


struct xz_rect : public hitable
{
	xz_rect() = delete;
	~xz_rect() = default;
	xz_rect(float _x0, float _x1, float _z0, float _z1, float _k, material& _rMaterial)
		:x0{_x0}, x1{_x1}, z0{_z0}, z1{_z1}, k{_k}, r_material{_rMaterial}
	{}

	bool hit(const ray& _r, float _tMin, float _tMax, hit_record& _rec) const override
	{
		float t = (k - _r.origin().y()) / _r.direction().y();
		if (!(t == t))
		{
			//std::cout << "NaN found in xz_rect::hit" << std::endl;
			return false;
		}
		if (t < _tMin || t > _tMax)
			return false;
		float x = _r.origin().x() + t*_r.direction().x();
		float z = _r.origin().z() + t*_r.direction().z();
		if (x < x0 || x > x1 || z < z0 || z > z1)
			return false;
		_rec.u = (x-x0)/(x1-x0);
		_rec.v = (z-z0)/(z1-z0);
		_rec.t = t;
		_rec.p_material = &r_material;
		_rec.p = _r.point_at_parameter(t);
		_rec.normal = vec3{0.f, 1.f, 0.f};
		return true;
	}
	bool bounding_box(float _t0, float _t1, aabb& _box) const override
	{
		_box = aabb{vec3{x0, k-.0001f, z0}, vec3{x1, k+.0001f, z1}};
		return true;
	}
	float pdf_value(const vec3& _origin, const vec3& _v) const override
	{
		hit_record rec;
		if (hit(ray{_origin, _v}, .001f, FLT_MAX, rec))
		{
			float area = (x1-x0)*(z1-z0);
			float distance_squared = rec.t * rec.t * _v.squared_length();
			float cosine = fabsf(dot(_v, rec.normal) / _v.length());
			float result = distance_squared / (cosine * area);
			return result;
		}
		else
			return 0.f;
	}
	vec3 random(const vec3& _origin) const override
	{
		float x = x0 + random::sample()*(x1 - x0);
		float z = z0 + random::sample()*(z1 - z0);
		vec3 random_point{x, k, z};
		return random_point - _origin;
	}

	float x0, x1, z0, z1, k;
	material& r_material;
};


struct yz_rect : public hitable
{
	yz_rect() = delete;
	~yz_rect() = default;
	yz_rect(float _y0, float _y1, float _z0, float _z1, float _k, material& _rMaterial)
		:y0{_y0}, y1{_y1}, z0{_z0}, z1{_z1}, k{_k}, r_material{_rMaterial}
	{}

	bool hit(const ray& _r, float _tMin, float _tMax, hit_record& _rec) const
	{
		float t = (k - _r.origin().x()) / _r.direction().x();
		if (!(t == t))
		{
			//std::cout << "NaN found in yz_rect::hit" << std::endl;
			return false;
		}
		if (t < _tMin || t > _tMax)
			return false;
		float y = _r.origin().y() + t*_r.direction().y();
		float z = _r.origin().z() + t*_r.direction().z();
		if (y < y0 || y > y1 || z < z0 || z > z1)
			return false;
		_rec.u = (y - y0) / (y1 - y0);
		_rec.v = (z - z0) / (z1 - z0);
		_rec.t = t;
		_rec.p_material = &r_material;
		_rec.p = _r.point_at_parameter(t);
		_rec.normal = vec3{1.f, 0.f, 0.f};
		return true;
	}
	bool bounding_box(float _t0, float _t1, aabb& _box) const
	{
		_box = aabb{vec3{k-.0001f, y0, z0}, vec3{k+.0001f, y1, z1}};
		return true;
	}

	float y0, y1, z0, z1, k;
	material& r_material;
};


#endif // __YS_AA_RECTANGLES_HPP__
