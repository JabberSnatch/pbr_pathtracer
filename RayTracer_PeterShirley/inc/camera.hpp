#ifndef __YS_CAMERA_HPP__
#define __YS_CAMERA_HPP__

#include "ray.hpp"
#include "random.hpp"


struct camera
{
	camera() = default;
	camera(const vec3& _position, const vec3& _target, const vec3& _up,
		   float _verticalFov, float _aspect, float _aperture, float _focusDistance,
		   float _t0, float _t1)
		:origin{ _position }, time0{_t0}, time1{_t1}
	{
		lens_radius = _aperture * .5f;
		float theta = _verticalFov * 3.1415926535f / 180.f;
		float half_height = tan(theta * .5f);
		float half_width = _aspect * half_height;
		w = unit_vector(origin - _target);
		u = unit_vector(cross(_up, w));
		v = cross(w, u);
		lower_left_corner = origin - 
							half_width*_focusDistance*u - 
							half_height*_focusDistance*v -
							_focusDistance*w;
		horizontal = 2.f*half_width*_focusDistance*u;
		vertical = 2.f*half_height*_focusDistance*v;
	}

	ray get_ray(float _u, float _v) {
		//return ray{origin, lower_left_corner + _u*horizontal + _v*vertical - origin};
		vec3 random_lens_position = lens_radius * random::in_unit_disk();
		vec3 offset = u*random_lens_position.x() + v*random_lens_position.y();
		float time = time0 + random::sample()*(time1 - time0);
		return ray{origin + offset, 
				   lower_left_corner + _u*horizontal + _v*vertical - origin - offset,
				   time};
	}

	vec3 origin;
	vec3 lower_left_corner;
	vec3 horizontal;
	vec3 vertical;
	vec3 u, v, w;
	float lens_radius;
	float time0, time1;
};


#endif // __YS_CAMERA_HPP__
