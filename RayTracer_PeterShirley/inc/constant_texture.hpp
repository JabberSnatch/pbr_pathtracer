#ifndef __YS_TEXTURE_HPP__
#define __YS_TEXTURE_HPP__

#include "texture.hpp"


struct constant_texture : public texture
{
	constant_texture() = default;
	constant_texture(const vec3& _color)
		:color{_color}
	{}

	vec3 value(float _u, float _v, const vec3& _position) const override
	{
		return color;
	}

	vec3 color;
};


#endif // __YS_TEXTURE_HPP__
