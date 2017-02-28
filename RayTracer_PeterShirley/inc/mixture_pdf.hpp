#ifndef __YS_MIXTURE_PDF_HPP__
#define __YS_MIXTURE_PDF_HPP__

#include <functional>
#include "pdf.hpp"
#include "random.hpp"


struct mixture_pdf : public pdf
{
	mixture_pdf(pdf& _p0, pdf& _p1)
		:p{std::ref(_p0), std::ref(_p1)}
	{}
	
	float value(const vec3& _direction) const override
	{
		return .5f * p[0].get().value(_direction) + .5f * p[1].get().value(_direction);
	}

	vec3 generate() const override
	{
		if (g_RNG.sample() < .5f)
			return p[0].get().generate();
		else
			return p[1].get().generate();
	}

	std::reference_wrapper<pdf> p[2];
};


#endif // __YS_MISTURE_PDF_HPP__
