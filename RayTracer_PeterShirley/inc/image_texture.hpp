#ifndef __YS_IMAGE_TEXTURE_HPP__
#define __YS_IMAGE_TEXTURE_HPP__

#include "texture.hpp"


inline int ficlamp(int _v, int _min, int _max)
{
	_v = _v < _min ? _min : _v;
	_v = _v > _max ? _max : _v;
	return _v;
}


struct image_texture : public texture
{
	using uchar = unsigned char;

	image_texture() = default;
	image_texture(uchar *_pixels, int _width, int _height)
		:data{_pixels}, width{_width}, height{_height}
	{}

	vec3 value(float _u, float _v, const vec3& _p) const override;

	uchar *data;
	int width, height;
};


vec3
image_texture::value(float _u, float _v, const vec3& _p) const
{
	int i = int(_u * width);
	int j = int(_v * height);
	i = ficlamp(i, 0, width);
	j = ficlamp(j, 0, height);
	float r = int(data[3 * i + 3 * width * j    ]) / 255.f;
	float g = int(data[3 * i + 3 * width * j + 1]) / 255.f;
	float b = int(data[3 * i + 3 * width * j + 2]) / 255.f;
	return vec3{r, g, b};
}


#endif // __YS_IMAGE_TEXTURE_HPP__
