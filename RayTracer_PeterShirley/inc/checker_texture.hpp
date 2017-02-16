#ifndef __YS_CHECKER_TEXTURE_HPP__
#define __YS_CHECKER_TEXTURE_HPP__

#include "texture.hpp"


struct checker_texture : public texture
{
	checker_texture() = delete;
	~checker_texture() = default;
	checker_texture(texture& _odd, texture& _even)
		:odd{_odd}, even{_even}
	{}

	vec3 value(float _u, float _v, const vec3& _p) const
	{
		float sines = sin(10.f * _p.x()) * sin(10.f * _p.y()) * sin(10.f * _p.z());
		if (sines > 0.f)
			return odd.value(_u, _v, _p);
		else
			return even.value(_u, _v, _p);
	}


	texture& odd;
	texture& even;
};


#endif // __YS_CHECKER_TEXTURE_HPP__
