#ifndef __YS_BEZIER_PATCH_HPP__
#define __YS_BEZIER_PATCH_HPP__

#include "triangle.hpp"
#include "hitable_list.hpp"
#include "lifetime_spreader.hpp"


namespace bezier
{
	constexpr inline int
	factorial(int _n)
	{
		int fact {1};
		for (int i {2}; i <= _n; ++i) fact *= i;
		return fact;
	}
	
	inline float
	bernstein(float _t, int _i, int _n)
	{
		float const one_minus_t {1.f - _t};
		float const Cni {factorial(_n) / float(factorial(_i) * factorial(_n - _i))};

		return Cni * pow(_t, float(_i)) * pow(1.f - _t, float(_n - _i));
	}
}


struct bezier_patch : public hitable
{
	bezier_patch() = delete;
	bezier_patch(std::vector<vec3> &&_control_points, int _div_count, material &_rMaterial)
		:control_points {_control_points}
	{
		int const			vertex_stride {_div_count + 1};
		std::vector<vec3>	vertices {};
		vertices.resize(vertex_stride * vertex_stride);

		for (int i {0}; i < vertex_stride; ++i)
		{
			for (int j {0}; j < vertex_stride; ++j)
			{
				float const s {i / float(_div_count)};
				float const t {j / float(_div_count)};

				vertices[i * vertex_stride + j] = P(s, t);
			}
		}


		mesh.list.reserve(_div_count * _div_count * 2);
		for (int i {0}; i < _div_count; ++i)
		{
			for (int j {0}; j < _div_count; ++j)
			{
				int const next_i {i + 1};
				int const next_j {j + 1};

				vec3 A {vertices[i * vertex_stride + j]};
				vec3 B {vertices[i * vertex_stride + next_j]};
				vec3 C {vertices[next_i * vertex_stride + next_j]};
				vec3 D {vertices[next_i * vertex_stride + j]};

				hitable& h1 {triangle_spreader.add_object<hitable>(
					new triangle(A, B, C, _rMaterial)
				)};
				hitable& h2 {triangle_spreader.add_object<hitable>(
					new triangle(A, C, D, _rMaterial)
				)};
				
				mesh.list.push_back(std::ref(h1));
				mesh.list.push_back(std::ref(h2));
			}
		}
	}


	bool hit(const ray& _r, float _tMin, float _tMax, hit_record& _rec) const
	{
		return mesh.hit(_r, _tMin, _tMax, _rec);
	}
	bool bounding_box(float _t0, float _t1, aabb& _box) const
	{
		return mesh.bounding_box(_t0, _t1, _box);
	}


	vec3	P(float _s, float _t) const
	{
		vec3 point {vec3::zero()};

		for (int i {0}; i < 4; ++i)
		{
			for (int j {0}; j < 4; ++j)
			{
				float const bernstein_i {bezier::bernstein(_s, i, 3)};
				float const bernstein_j {bezier::bernstein(_t, j, 3)};
				point += control_points[i + j * 4] * bernstein_i * bernstein_j;
			}
		}

		return point;
	}


	hitable_list				mesh;
	std::vector<vec3>			control_points;
	lifetime_spreader<hitable>	triangle_spreader;
};

#endif // __YS_BEZIER_PATCH_HPP__
