#ifndef __YS_HITABLE_LIST_HPP__
#define __YS_HITABLE_LIST_HPP__

#include <vector>
#include <memory>
#include <functional>
#include "hitable.hpp"

struct hitable_list : public hitable
{
	hitable_list() = default;
	hitable_list(const std::vector<std::reference_wrapper<hitable>>& _list)
		:list{_list}
	{}
	bool hit(const ray& _r, float _tMin, float _tMax, hit_record& _rec) const override;
	bool bounding_box(float _t0, float _t1, aabb& _box) const override;
	float pdf_value(const vec3& _origin, const vec3& _v) const override;
	vec3 to_random_position(const vec3& _origin) const override;

	std::vector<std::reference_wrapper<hitable>> list;
};


bool
hitable_list::hit(const ray& _r, float _tMin, float _tMax, hit_record& _rec) const
{
	hit_record temp_record;
	bool hit_anything = false;
	float closest_so_far = _tMax;
	for (auto item : list)
	{
		if (item.get().hit(_r, _tMin, closest_so_far, temp_record))
		{
			hit_anything = true;
			closest_so_far = temp_record.t;
			_rec = temp_record;
		}
	}
	return hit_anything;
}


bool
hitable_list::bounding_box(float _t0, float _t1, aabb& _box) const
{
	if (list.size() < 1) return false;

	aabb temp_box;
	bool first_true = list[0].get().bounding_box(_t0, _t1, temp_box);
	if (!first_true)
		return false;
	else
		_box = temp_box;

	for (size_t i = 1; i < list.size(); ++i)
	{
		if (list[i].get().bounding_box(_t0, _t1, temp_box))
			_box = aabb{_box, temp_box};
		else
			return false;
	}

	return true;
}


float
hitable_list::pdf_value(const vec3& _origin, const vec3& _v) const
{
	float weight = 1.f / list.size();
	float sum = 0.f;
	for (hitable& element : list)
		sum += weight * element.pdf_value(_origin, _v);

	return sum;
}


vec3
hitable_list::to_random_position(const vec3& _origin) const
{
	int index = int(g_RNG.sample() * list.size());
	index = index >= list.size() ? int(list.size() - 1) : index;
	return list[index].get().to_random_position(_origin);
}


#endif // __YS_HITABLE_LIST_HPP__
