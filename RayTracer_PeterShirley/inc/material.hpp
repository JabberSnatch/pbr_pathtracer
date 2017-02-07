#ifndef __YS_MATERIAL_HPP__
#define __YS_MATERIAL_HPP__

#include "hitable.hpp"


struct material
{
	virtual ~material() = default;
	virtual bool scatter(const ray& _rayIn, 
						 const hit_record& _record, 
						 vec3& _attenuation, 
						 ray& _rayOut) const = 0;
};


#endif // __YS_MATERIAL_HPP__
