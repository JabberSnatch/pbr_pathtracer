#ifndef __YS_NOISE_TEXTURE_HPP__
#define __YS_NOISE_TEXTURE_HPP__

#include "texture.hpp"
#include "perlin.hpp"


struct noise_texture : public texture
{
	noise_texture() = delete;
	~noise_texture() = default;
	noise_texture(float _scale)
		:scale{_scale}
	{}

	vec3 value(float _u, float _v, const vec3& _p) const override
	{
		vec3 sp{scale*_p};
		//return vec3::one() * 0.5f * (1.f + perlin::noise(sp));
		//return vec3::one() * perlin::turb(scale * _p);
		//return vec3::one() * 0.5f * (1.f + sin(sp.z() + 10.f * perlin::turb(sp)));
		return vec3::one() * 0.5f * (1.f + sin(sp.z() + 10.f * perlin::turb(_p)));
	}

	float scale;
};


#endif // __YS_NOISE_TEXTURE_HPP__
