#ifndef __YS_COSINE_PDF_HPP__
#define __YS_COSINE_PDF_HPP__

#include "pdf.hpp"
#include "orthonormal_basis.hpp"
#include "random.hpp"


struct cosine_pdf : public pdf
{
	cosine_pdf(const vec3& _w)
		:uvw{_w}
	{}

	float value(const vec3& _direction) const override
	{
		float cosine = dot(unit_vector(_direction), uvw.w());
		if (!(cosine == cosine))
			std::cout << "NaN found in cosine_pdf" << std::endl;

		if (cosine > 0.f)
			return cosine / 3.1415926535f;
		else
			return 0.f;
	}

	vec3 generate() const override
	{
		return uvw.local(g_RNG.cosine_direction());
	}

	
	onb uvw;
};


#endif // __YS_COSINE_PDF_HPP__
