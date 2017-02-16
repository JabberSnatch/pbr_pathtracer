#ifndef __YS_HITABLE_PDF_HPP__
#define __YS_HITABLE_PDF_HPP__

#include "pdf.hpp"
#include "hitable.hpp"


struct hitable_pdf : public pdf
{
	hitable_pdf() = delete;
	hitable_pdf(hitable& _rHitable, const vec3& _origin)
		:r_hitable{_rHitable}, origin{_origin}
	{}

	float value(const vec3& _direction) const override
	{
		float value = r_hitable.pdf_value(origin, _direction);
		if (!(value == value))
			std::cout << "NaN found in hitable_pdf" << std::endl;
		return value;
	}

	vec3 generate() const override
	{
		return r_hitable.random(origin);
	}

	hitable& r_hitable;
	vec3 origin;
};


#endif // __YS_HITABLE_PDF_HPP__
