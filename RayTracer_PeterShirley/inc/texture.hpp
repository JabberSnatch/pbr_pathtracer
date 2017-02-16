#ifndef __YS_TEXTURE_HPP__
#define __YS_TEXTURE_HPP__

#include "vec3.hpp"


struct texture
{
	virtual ~texture() = default;
	virtual vec3 value(float _u, float _v, const vec3& _position) const = 0;
};


#endif // __YS_TEXTURE_HPP__
