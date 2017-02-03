#ifndef __YS_AABB_HPP__
#define __YS_AABB_HPP__



// NOTE: We use these instead of builtin min/max because we do not care about exceptions.
inline float ffmin(float _a, float _b) { return _a < _b ? _a : _b; }
inline float ffmax(float _a, float _b) { return _a > _b ? _a : _b; }

struct aabb
{
	aabb() = default;
	aabb(const vec3& _min, const vec3& _max)
		:min{_min}, max{_max}
	{}
	aabb(const aabb& _a, const aabb& _b)
		:min{fminf(_a.min.x(), _b.min.x()), 
			 fminf(_a.min.y(), _b.min.y()), 
			 fminf(_a.min.z(), _b.min.z())},
		 max{fmaxf(_a.max.x(), _b.max.x()),
			 fmaxf(_a.max.y(), _b.max.y()),
			 fmaxf(_a.max.z(), _b.max.z())}
	{}

	bool hit(const ray& _r, float _tMin, float _tMax) const
	{
		for (int a = 0; a < 3; ++a)
		{
			float min_plane_intersection = (min[a] - _r.origin()[a]) / _r.direction()[a];
			float max_plane_intersection = (max[a] - _r.origin()[a]) / _r.direction()[a];
			float t0 = ffmin(min_plane_intersection, max_plane_intersection);
			float t1 = ffmax(min_plane_intersection, max_plane_intersection);

			_tMin = ffmax(t0, _tMin);
			_tMax = ffmin(t1, _tMax);
			if (_tMax <= _tMin)
				return false;
		}
		return true;

	}
	// NOTE: Andrew Kensler's compiler optimized version
	/*
	inline bool hit(const ray& _r, float _tMin, float _tMax) const
	{
		for(int a = 0; a < 3; ++a)
		{
			float invD = 1.f / r.direction()[a];
			float t0 = (min[a] - r.origin()[a]) * invD;
			float t1 = (max[a] - r.origin()[a]) * invD;
			if (invD < .0f)
				std::swap(t0, t1);
			_tMin = t0 > _tMin ? t0 : _tMin;
			_tMax = t1 < _tMax ? t1 : _tMax;
			if (_tMax <= _tMin)
				return false;
		}
		return true;
	}
	*/

	vec3 min;
	vec3 max;
};


#endif // __YS_AABB_HPP__
