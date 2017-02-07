#include <iostream>
#include <fstream>
#include <limits>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <memory>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "sphere.hpp"
#include "hitable_list.hpp"
#include "camera.hpp"
#include "random.hpp"
#include "lambertian.hpp"
#include "metal.hpp"
#include "dielectric.hpp"
#include "moving_sphere.hpp"
#include "bvh_node.hpp"
#include "lifetime_spreader.hpp"



float 
hit_sphere(const vec3& _center, float _radius, const ray& _r)
{
	vec3 oc = _r.origin() - _center;
	float a = dot(_r.direction(), _r.direction());
	float b = 2.f * dot(oc, _r.direction());
	float c = dot(oc, oc) - _radius*_radius;
	float discriminant = b*b - 4 * a*c;
	if (discriminant < 0.f)
		return -1.0f;
	else
		return (-b - sqrt(discriminant)) / (2.f * a);
}


void
add_random_spheres(hitable_list& _scene, int _count, const vec3& _bounds, 
				   lifetime_spreader<hitable>& _hitable_spreader,
				   lifetime_spreader<material>& _material_spreader)
{
	for (int i = 0; i < _count; ++i)
	{
		float material_pick = random::sample();
		vec3 position{(random::ninja() * 2.f - vec3::one()) * _bounds + vec3{.0f, .2f, .0f}};
		
		if (material_pick < .8f)
		{
			_scene.list.push_back(std::ref(_hitable_spreader.add_object<hitable>(
				new sphere{ position,
						   .2f,
						   _material_spreader.add_object<lambertian>(
							   new lambertian{random::ninja() * random::ninja()})
				}
			)));
		}
		else if (material_pick < .95f)
		{
			_scene.list.push_back(std::ref(_hitable_spreader.add_object<hitable>(
				new sphere{ position,
						   .2f,
						   _material_spreader.add_object<metal>( 
							   new metal{(random::ninja() + vec3::one()) * .5f,
										 random::sample() * .5f})
				}
			)));
		}
		else
		{
			_scene.list.push_back(std::ref(_hitable_spreader.add_object<hitable>(
				new sphere{ position,
						   .2f,
						   _material_spreader.add_object<dielectric>(
							   new dielectric{1.5f})
				}
			)));
		}
	}
}


vec3
color(const ray& _r, hitable& _world, int _depth)
{
	hit_record record;
	if (_world.hit(_r, 0.001f, std::numeric_limits<float>::max(), record))
	{
		ray scattered;
		vec3 attenuation;
		if (_depth < 50 && record.p_material->scatter(_r, record, attenuation, scattered))
			return attenuation * color(scattered, _world, _depth + 1);
		else
			return vec3{0.f, 0.f, 0.f};
		//vec3 target{record.p + record.normal + random_in_unit_sphere()};
		//return .5f * (vec3::one() + record.normal);
		//return .5f * color(ray{ record.p, target - record.p }, _world);
	}
	else
	{
		vec3 unit_direction = unit_vector(_r.direction());
		float t = 0.5f * (unit_direction.y() + 1.0f);
		return (1.0f - t) * vec3::one() + t * vec3{ .5f, .7f, 1.f };
	}
}


int 
main()
{
	const int image_width = 640;
	const int image_height = 360;
	const int sample_count = 100;

	lifetime_spreader<hitable> hitable_spreader{};
	lifetime_spreader<material> material_spreader{};

	hitable_list& world{hitable_spreader.add_object<hitable_list>(new hitable_list{})};
#if 1
	world.list.push_back(std::ref(hitable_spreader.add_object<hitable>(
		new sphere{vec3{0.f, .5f, -2.5f},
				   .5f,
				   material_spreader.add_object<material>(
					   new metal{vec3{1.f, 1.f, 1.f}, .0f})
		}
	)));
	world.list.push_back(std::ref(hitable_spreader.add_object<hitable>(
		new sphere{vec3{0.f, 0.5f, .5f},
				   .5f,
				   material_spreader.add_object<material>(
					   new metal{vec3{1.f, 1.f, 1.f}, 0.f})
		}
	)));
	world.list.push_back(std::ref(hitable_spreader.add_object<hitable>(
		new sphere{vec3{.0f, -1000.f, -1.f},
				   1000.f,
				   material_spreader.add_object<material>(
					   new lambertian{vec3{.8f, .8f, .0f}})
		}
	)));
	world.list.push_back(std::ref(hitable_spreader.add_object<hitable>(
		new sphere{vec3{1.1f, 0.5f, -1.f},
				   .5f,
				   material_spreader.add_object<material>(
					   new dielectric{1.5f})
		}
	)));
	world.list.push_back(std::ref(hitable_spreader.add_object<hitable>(
		new sphere{vec3{1.1f, 0.5f, -1.f},
				   -.49f,
				   material_spreader.add_object<material>(
					   new dielectric{1.5f})
		}
	)));
	world.list.push_back(std::ref(hitable_spreader.add_object<hitable>(
		new sphere{vec3{-1.1f, 0.5f, -1.f},
				   .5f,
				   material_spreader.add_object<material>(
					   new metal{vec3{.6f, .8f, .8f}, 0.3f})
		}
	)));
	world.list.push_back(std::ref(hitable_spreader.add_object<hitable>(
		new moving_sphere{vec3{0.f, .5f, -1.f}, vec3{0.f, 1.f, -1.f},
						  .0f, 1.f,
						   .5f,
						   material_spreader.add_object<material>(
							   new lambertian{vec3{.980f, .854f, .368f}})
		}
	)));
	
	add_random_spheres(world, 100, vec3{10.f, 0.f, 10.f},
					   hitable_spreader, material_spreader);

	bvh_node bvh_world{world.list, 0.f, 1.f, hitable_spreader};

	//camera main_camera{ origin, lower_left_corner, horizontal, vertical };
	vec3 view_position{2.f, .4f, -1.3f};
	vec3 look_position{0.f, .75f, -1.f};
	float dist_to_focus = (view_position - look_position).length();
	float aperture = 0.f;
	camera main_camera(view_position, look_position, vec3{.0f, 1.f, .0f},
					   90.f, float(image_width) / float(image_height), aperture,
					   dist_to_focus, 0.f, 1.f);
#else
	float R = cos(3.1415926535f * .25f);
	world->list.push_back(new sphere{
								vec3{-R, .0f, -1.f},
								R,
								new lambertian{vec3{.0f, .0f, 1.f}}});
	world->list.push_back(new sphere{
								vec3{R, .0f, -1.f},
								R,
								new lambertian{vec3{1.f, .0f, .0f}}});

	camera main_camera{90.f, float(image_width) / float(image_height)};
#endif

	auto begin = std::chrono::system_clock::now();

	unsigned char* pixels = new unsigned char[image_height * image_width * 3]{};
	unsigned char* head = pixels;
	for (int j = image_height - 1; j >= 0; j--)
	{
		for (int i = 0; i < image_width; i++)
		{
			vec3 col{0.f, 0.f, 0.f};
			for (int s = 0; s < sample_count; ++s)
			{
				float u = (float(i) + random::sample()) / float(image_width);
				float v = (float(j) + random::sample()) / float(image_height);

				ray r{main_camera.get_ray(u, v)};

				col += color(r, bvh_world, 0);
			}

			col /= float(sample_count);
			col = vec3{sqrt(col.r()), sqrt(col.g()), sqrt(col.b())};

			int ir = int(255.99f * col.r());
			int ig = int(255.99f * col.g());
			int ib = int(255.99f * col.b());

			*head = static_cast<unsigned char>(ir); 
			head++;
			*head = static_cast<unsigned char>(ig); 
			head++;
			*head = static_cast<unsigned char>(ib); 
			head++;
		}
		std::cout << "Row " << j << " done" << std::endl;
	}

	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed = end - begin;

	std::cout << "Elapsed time : " << elapsed.count() << std::endl;

	std::stringstream ss{};
	std::time_t now = std::time(nullptr);
	std::tm tm = *std::localtime(&now);
	ss << std::put_time(&tm, "%y%m%d_%H%M%S_");
	std::string png_path{ss.str() + std::to_string(sample_count) +".png"};
	stbi_write_png(png_path.c_str(), 
				   image_width, image_height, 3, pixels, image_width * 3);

	delete[] pixels;

	std::string buf{};
	std::cin >> buf;
}
