#ifndef __YS_BVH_NODE_HPP__
#define __YS_BVH_NODE_HPP__

#include <algorithm>
#include "lifetime_spreader.hpp"
#include "hitable.hpp"


struct _box_comparator
{
	_box_comparator() = delete;
	_box_comparator(float _t0, float _t1, int _vector_index)
		:t0{_t0}, t1{_t1}, vector_index{_vector_index}
	{}

	bool operator () (std::reference_wrapper<hitable> _a, std::reference_wrapper<hitable> _b)
	{ return box_compare_index(_a, _b, t0, t1, vector_index); }

	bool
	box_compare_index(hitable& _a, hitable& _b, float _t0, float _t1, int _vector_index)
	{
		aabb box_left, box_right;
		if (!_a.bounding_box(_t0, _t1, box_left) || !_b.bounding_box(_t0, _t1, box_right))
			std::cerr << "no bounding box in bvh_node constructor\n";
		if (box_left.min.e[_vector_index] - box_right.min.e[_vector_index] < 0.f)
			return true;
		else
			return false;
	}

	float t0;
	float t1;
	int vector_index;
};


struct bvh_node : public hitable
{
	bvh_node() = default;
	bvh_node(std::vector<std::reference_wrapper<hitable>> _list, float _t0, float _t1,
			 lifetime_spreader<hitable>& _spreader);
	bool hit(const ray& _r, float _tMin, float _tMax, hit_record& _record) const override;
	bool bounding_box(float _t0, float _t1, aabb& _box) const override;

	hitable* r_left;
	hitable* r_right;
	aabb box;
};


bvh_node::bvh_node(std::vector<std::reference_wrapper<hitable>> _list, float _t0, float _t1,
				   lifetime_spreader<hitable>& _spreader)
{
	{ // NOTE: sort the list along a random axis
		_box_comparator comparer{_t0, _t1, int(3.f * random::sample())};
		std::sort(_list.begin(), _list.end(), comparer);
	}

	if (_list.size() == 1)
	{
		r_left = r_right = &_list[0].get();
	}
	else if (_list.size() == 2)
	{
		r_left = &_list[0].get();
		r_right = &_list[1].get();
	}
	else
	{
		auto half_iterator = _list.begin() + _list.size() / 2;
		r_left = &_spreader.add_object<hitable>(
					new bvh_node{{_list.begin(), half_iterator}, _t0, _t1, _spreader}
		);
		r_right = &_spreader.add_object<hitable>(
					new bvh_node{{half_iterator, _list.end()}, _t0, _t1, _spreader}
		);
	}

	aabb box_left, box_right;
	if (!r_left->bounding_box(_t0, _t1, box_left) || !r_right->bounding_box(_t0, _t1, box_right))
		std::cerr << "no bounding box in bvh_node constructor\n";
	box = aabb{box_left, box_right};
}


bool
bvh_node::hit(const ray& _r, float _tMin, float _tMax, hit_record& _record) const
{
	if (box.hit(_r, _tMin, _tMax))
	{
		hit_record left_record, right_record;
		bool hit_left = r_left->hit(_r, _tMin, _tMax, left_record);
		bool hit_right = r_right->hit(_r, _tMin, _tMax, right_record);
		
		if (hit_left && hit_right)
		{
			if (left_record.t < right_record.t)
				_record = left_record;
			else
				_record = right_record;
			return true;
		}
		else if (hit_left)
		{
			_record = left_record;
			return true;
		}
		else if (hit_right)
		{
			_record = right_record;
			return true;
		}
		else
			return false;
	}
	else
		return false;
}


bool
bvh_node::bounding_box(float _t0, float _t1, aabb& _box) const
{
	_box = box;
	return true;
}


#endif // __YS_BVH_NODE_HPP__
