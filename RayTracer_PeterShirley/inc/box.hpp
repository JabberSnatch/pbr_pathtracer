#ifndef __YS_BOX_HPP__
#define __YS_BOX_HPP__

#include "hitable.hpp"
#include "hitable_list.hpp"
#include "lifetime_spreader.hpp"
#include "aa_rectangles.hpp"
#include "flip_normals.hpp"


struct box : public hitable
{
	box() = default;
	~box() = default;
	box(const vec3& _min, const vec3& _max, material& _rMaterial)
		:min{_min}, max{_max}
	{
		faces.list.push_back(std::ref(internal_spreader.add_object<hitable>(
			new xy_rect{_min.x(), _max.x(), _min.y(), _max.y(), _max.z(), _rMaterial}
		)));
		faces.list.push_back(std::ref(internal_spreader.add_object<hitable>(
			new flip_normals{internal_spreader.add_object(
				new xy_rect{_min.x(), _max.x(), _min.y(), _max.y(), _min.z(), _rMaterial}
			)}
		)));
		faces.list.push_back(std::ref(internal_spreader.add_object<hitable>(
			new xz_rect{_min.x(), _max.x(), _min.z(), _max.z(), _max.y(), _rMaterial}
		)));
		faces.list.push_back(std::ref(internal_spreader.add_object<hitable>(
			new flip_normals{internal_spreader.add_object(
				new xz_rect{_min.x(), _max.x(), _min.z(), _max.z(), _min.y(), _rMaterial}
			)}
		)));
		faces.list.push_back(std::ref(internal_spreader.add_object<hitable>(
			new yz_rect{_min.y(), _max.y(), _min.z(), _max.z(), _max.x(), _rMaterial}
		)));
		faces.list.push_back(std::ref(internal_spreader.add_object<hitable>(
			new flip_normals{internal_spreader.add_object(
				new yz_rect{_min.y(), _max.y(), _min.z(), _max.z(), _min.x(), _rMaterial}
			)}
		)));
	}

	bool hit(const ray& _r, float _t0, float _t1, hit_record& _rec) const override
	{
		return faces.hit(_r, _t0, _t1, _rec);
	}
	bool bounding_box(float _t0, float _t1, aabb& _box) const override
	{
		_box = aabb{min, max};
		return true;
	}

	vec3 min, max;
	hitable_list faces;
	lifetime_spreader<hitable> internal_spreader;
};


#endif // __YS_BOX_HPP__
