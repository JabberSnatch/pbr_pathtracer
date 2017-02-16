#ifndef __YS_PERLIN_HPP__
#define __YS_PERLIN_HPP__

#include <cmath>
#include <vector>
#include "random.hpp"
#include "vec3.hpp"


inline float lerp(float _a, float _b, float _t)
{
	return (1.f-_t)*_a + _t*_b;
}


inline float trilinear_interp(float _c[2][2][2], float _u, float _v, float _w)
{
#define TRILINEAR_INTERP_UNROLLED_LOOP
#ifdef TRILINEAR_INTERP_UNROLLED_LOOP
	float C00 = lerp(_c[0][0][0], _c[1][0][0], _u);
	float C01 = lerp(_c[0][0][1], _c[1][0][1], _u);
	float C10 = lerp(_c[0][1][0], _c[1][1][0], _u);
	float C11 = lerp(_c[0][1][1], _c[1][1][1], _u);

	float C0 = lerp(C00, C10, _v);
	float C1 = lerp(C01, C11, _v);

	return lerp(C0, C1, _w);
#else
	float accum = 0;
	for (int i = 0; i < 2; ++i)
		for (int j = 0; j < 2; ++j)
			for (int k = 0; k < 2; ++k)
				accum +=	(i*_u + (1.f-i)*(1.f-_u))*
							(j*_v + (1.f-j)*(1.f-_v))*
							(k*_w + (1.f-k)*(1.f-_w))*
							_c[i][j][k];
	return accum;
#endif
}


inline float perlin_interp(vec3 _c[2][2][2], float _u, float _v, float _w)
{
	float uu = _u*_u*(3.f - 2.f*_u);
	float vv = _v*_v*(3.f - 2.f*_v);
	float ww = _w*_w*(3.f - 2.f*_w);
	
#define PERLIN_INTERP_UNROLLED_LOOP
#ifdef PERLIN_INTERP_UNROLLED_LOOP
	vec3 uvw{_u, _v, _w};
	
	float C00 = lerp(dot(_c[0][0][0], uvw - vec3{0.f, 0.f, 0.f}),
					 dot(_c[1][0][0], uvw - vec3{1.f, 0.f, 0.f}), _u);
	float C01 = lerp(dot(_c[0][0][1], uvw - vec3{0.f, 0.f, 1.f}),
					 dot(_c[1][0][1], uvw - vec3{1.f, 0.f, 1.f}), _u);
	float C10 = lerp(dot(_c[0][1][0], uvw - vec3{0.f, 1.f, 0.f}),
					 dot(_c[1][1][0], uvw - vec3{1.f, 1.f, 0.f}), _u);
	float C11 = lerp(dot(_c[0][1][1], uvw - vec3{0.f, 1.f, 1.f}),
					 dot(_c[1][1][1], uvw - vec3{1.f, 1.f, 1.f}), _u);

	float C0 = lerp(C00, C10, _v);
	float C1 = lerp(C01, C11, _v);

	return lerp(C0, C1, _w);
#else
	float accum = 0.f;
	float c[2][2][2];
	for (int i = 0; i < 2; ++i)
		for (int j = 0; j < 2; ++j)
			for (int k = 0; k < 2; ++k)
			{
				vec3 weight_v{_u-(float)i, _v-(float)j, _w-(float)k};
#if 1
				accum +=	(i*uu + (1.f-i)*(1.f-uu))*
							(j*vv + (1.f-j)*(1.f-vv))*
							(k*ww + (1.f-k)*(1.f-ww))*
							dot(_c[i][j][k], weight_v);
			}
	return (accum + 1.f) * 0.5f;
#else
				c[i][j][k] = dot(_c[i][j][k], weight_v);
			}

	float interpolated = trilinear_interp(c, uu, vv, ww);
	return (interpolated + 1.f) * 0.5f;
#endif
#endif
}


struct perlin
{
	// NOTE: This is on the [-1.f, 1.f] range.
	static float noise(const vec3& _p)
	{
		float u = _p.x() - floor(_p.x());
		float v = _p.y() - floor(_p.y());
		float w = _p.z() - floor(_p.z());

		int i = (int)floor(_p.x());
		int j = (int)floor(_p.y());
		int k = (int)floor(_p.z());
		vec3 c[2][2][2];
		for (int di = 0; di < 2; ++di)
			for (int dj = 0; dj < 2; ++dj)
				for (int dk = 0; dk < 2; ++dk)
					c[di][dj][dk] = ranvec[
										perm_x[(i+di) & 255]^ 
										perm_y[(j+dj) & 255]^
										perm_z[(k+dk) & 255]];
		return perlin_interp(c, u, v, w);
	}

	static float turb(const vec3& _p, int _octaves = 7)
	{
		float accum = 0.f;
		float weight = 1.f;
		vec3 temp_p = _p;
		for (int i = 0; i < _octaves; ++i)
		{
			accum += weight * noise(temp_p);
			weight *= 0.5f;
			temp_p *= 2.f;
		}
		return fabsf(accum);
	}

	static std::vector<vec3> generate()
	{
		std::vector<vec3> p(256);
		for (int i = 0; i < 256; ++i)
			p[i] = unit_vector(random::ninja()*2.f - vec3::one());
		return p;
	}

	static void permute(std::vector<int> &_p, int _n)
	{
		for (int i = _n-1; i > 0; --i)
		{
			int target = int(random::sample() * (i+1));
			std::swap(_p[i], _p[target]);
		}
	}

	static std::vector<int> generate_perm()
	{
		std::vector<int> p(256);
		for (int i = 0; i < 256; ++i)
			p[i] = i;
		permute(p, 256);
		return p;
	}

	static std::vector<vec3>	ranvec;
	static std::vector<int>		perm_x;
	static std::vector<int>		perm_y;
	static std::vector<int>		perm_z;
};


std::vector<vec3>	perlin::ranvec = perlin::generate();
std::vector<int>	perlin::perm_x = perlin::generate_perm();
std::vector<int>	perlin::perm_y = perlin::generate_perm();
std::vector<int>	perlin::perm_z = perlin::generate_perm();


#endif // __YS_PERLIN_HPP__
