#ifndef __YS_PDF_HPP__
#define __YS_PDF_HPP__

#include "vec3.hpp"


struct pdf
{
	virtual float	value(const vec3& _direction) const = 0;
	virtual vec3	generate() const = 0;
};


#endif // __YS_PDF_HPP__
