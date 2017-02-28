#include <iostream>
#include <fstream>
#include <limits>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <memory>
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
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
#include "texture.hpp"
#include "constant_texture.hpp"
#include "checker_texture.hpp"
#include "noise_texture.hpp"
#include "image_texture.hpp"
#include "texture_mapped_fuzziness_metal.hpp"
#include "diffuse_light.hpp"
#include "aa_rectangles.hpp"
#include "flip_normals.hpp"
#include "box.hpp"
#include "constant_medium.hpp"
#include "cosine_pdf.hpp"
#include "hitable_pdf.hpp"
#include "mixture_pdf.hpp"
#include "multiply.hpp"


#define IMAGE_WIDTH 640
#define IMAGE_HEIGHT 360
#define SAMPLE_COUNT 2000
#define MULTITHREADED
#define GROUP_WIDTH 8
#define GROUP_HEIGHT 8
#define NONUNIFORM_SAMPLING

//#define CORNELL_BOX
#define USUAL_SCENE
//#define SPAWN_RANDOM_SPHERES
#define RANDOM_SPHERE_COUNT 10
#define HOLLOWED_GLASS_BALL

#define IMPORTANCE_SAMPLING
#define COLOR_LIT_SCENE

#define GAUSSIAN_FILTER
//#define TRIANGLE_FILTER
//#define BOX_FILTER


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
				   lifetime_spreader<material>& _material_spreader,
				   lifetime_spreader<texture>& _texture_spreader)
{
	for (int i = 0; i < _count; ++i)
	{
		float material_pick = g_RNG.sample();
		vec3 position{(g_RNG.ninja() * 2.f - vec3::one()) * _bounds + vec3{.0f, .2f, .0f}};
		
		if (material_pick < .8f)
		{
			_scene.list.push_back(std::ref(_hitable_spreader.add_object<hitable>(
				new sphere{ position,
						   .2f,
						   _material_spreader.add_object<lambertian>(
							   new lambertian{
								   _texture_spreader.add_object<constant_texture>(
									   new constant_texture{
										   g_RNG.ninja() * g_RNG.ninja()}
								   )
							   }
						   )
				}
			)));
		}
		else if (material_pick < .95f)
		{
			_scene.list.push_back(std::ref(_hitable_spreader.add_object<hitable>(
				new sphere{ position,
						   .2f,
						   _material_spreader.add_object<metal>( 
							   new metal{(g_RNG.ninja() + vec3::one()) * .5f,
										 g_RNG.sample() * .5f})
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


void
cornell_box(hitable_list& _scene, hitable_list& _favoredHitables,
			lifetime_spreader<hitable>& _hitable_spreader,
			lifetime_spreader<material>& _material_spreader,
			lifetime_spreader<texture>& _texture_spreader)
{
	texture& red{_texture_spreader.add_object(
		new constant_texture{vec3{.65f, .05f, .05f}}
	)};
	texture& white{_texture_spreader.add_object(
		new constant_texture{vec3{.73f, .73f, .73f}}
	)};
	texture& green{_texture_spreader.add_object(
		new constant_texture{vec3{.12f, .45f, .15f}}
	)};
	texture& intense_green{_texture_spreader.add_object(
		new constant_texture{vec3{.12f, .45f, .15f} * 15.f}
	)};
	texture& orange{_texture_spreader.add_object(
		new constant_texture{vec3{1.f, 0.345f, .0f}}
	)};
	texture& blue{_texture_spreader.add_object(
		new constant_texture{vec3{0.f, .564f, .686f}}
	)};
	texture& powerful_white{_texture_spreader.add_object(
		new constant_texture{vec3{7.f, 7.f, 7.f}}
	)};
	texture& noise{_texture_spreader.add_object(
		new noise_texture{50.f}
	)};

	material& lambert_red{_material_spreader.add_object(
		new lambertian{orange}
	)};
	material& lambert_white{_material_spreader.add_object(
		new lambertian{white}
	)};
	material& lambert_green{_material_spreader.add_object(
		new lambertian{blue}
	)};
	material& light{_material_spreader.add_object(
		new diffuse_light{powerful_white}
	)};
	material& smaller_light{_material_spreader.add_object(
		new diffuse_light{intense_green}
	)};
	material& aluminum{_material_spreader.add_object(
		new metal{vec3{.8f, .85f, .68f}, .0f}
	)};
	material& glass{_material_spreader.add_object(
		new dielectric{1.5f}
	)};
	
	_scene.list.push_back(std::ref(_hitable_spreader.add_object<hitable>(
		new flip_normals{_hitable_spreader.add_object(
			new yz_rect{.0f, 555.f, .0f, 555.f, 555.f, lambert_green}
		)}
	)));
	_scene.list.push_back(std::ref(_hitable_spreader.add_object<hitable>(
		new yz_rect{.0f, 555.f, .0f, 555.f, 0.f, lambert_red}
	)));
	_scene.list.push_back(std::ref(_hitable_spreader.add_object<hitable>(
		new flip_normals{_hitable_spreader.add_object(
			new xz_rect{0.f, 555.f, 0.f, 555.f, 555.f, lambert_white}
		)}
	)));
	_scene.list.push_back(std::ref(_hitable_spreader.add_object<hitable>(
		new xz_rect{0.f, 555.f, 0.f, 555.f, 0.f, lambert_white}
	)));
	_scene.list.push_back(std::ref(_hitable_spreader.add_object<hitable>(
		new flip_normals{_hitable_spreader.add_object(
			new xy_rect{0.f, 555.f, 0.f, 555.f, 555.f, lambert_white}
		)}
	)));
	
	hitable& s1{_hitable_spreader.add_object<hitable>(
		new sphere{vec3{272.f, 450.f, 272.f}, 75.f, lambert_white}
	)};
	//_scene.list.push_back(std::ref(s1));

	hitable& b1{_hitable_spreader.add_object<hitable>(
		new box{vec3{0.f, 0.f, 0.f}, vec3{555.f, 555.f, 555.f}, lambert_white}
	)};
	//_scene.list.push_back(std::ref(_hitable_spreader.add_object<hitable>(
	//	new constant_medium{b1, .001f, white}
	//)));
	//_scene.list.push_back(std::ref(b1));
	
	hitable& aluminum_box{_hitable_spreader.add_object<hitable>(
		new box{vec3{265.f, 50.f, 295.f}, vec3{430.f, 330.f, 460.f}, glass}
	)};
	_scene.list.push_back(std::ref(aluminum_box));
	//_favoredHitables.list.push_back(std::ref(aluminum_box));

	hitable& glass_sphere{_hitable_spreader.add_object(
		new sphere{vec3{190.f, 90.f, 190.f}, 90.f, glass}
	)};
	_scene.list.push_back(std::ref(glass_sphere));
	_favoredHitables.list.push_back(std::ref(glass_sphere));
	
	hitable& inside_sphere{_hitable_spreader.add_object(
		new sphere{vec3{190.f, 90.f, 190.f}, -85.f, glass}
	)};
	//_scene.list.push_back(std::ref(inside_sphere));
	
	hitable& light_sphere{_hitable_spreader.add_object(
		new sphere{vec3{190.f, 90.f, 190.f}, 10.f, smaller_light}
	)};
	//_scene.list.push_back(std::ref(light_sphere));
	//_favoredHitables.list.push_back(std::ref(light_sphere));

	hitable& ceiling_light{_hitable_spreader.add_object<hitable>(
		new flip_normals{_hitable_spreader.add_object(
			new xz_rect{163.f, 393.f, 177.f, 382.f, 554.f, light}
		)}
	)};
	_scene.list.push_back(std::ref(ceiling_light));
	_favoredHitables.list.push_back(std::ref(ceiling_light));
}



void
usual_scene(hitable_list& _scene, hitable_list& _favoredHitables,
			lifetime_spreader<hitable>& _hitable_spreader,
			lifetime_spreader<material>& _material_spreader,
			lifetime_spreader<texture>& _texture_spreader)
{
	texture& black{_texture_spreader.add_object(
		new constant_texture{vec3{.0f, .0f, .0f}}
	)};
	texture& white{_texture_spreader.add_object(
		new constant_texture{vec3{1.f, 1.f, 1.f}}
	)};
	texture& solid_yellow{_texture_spreader.add_object(
		new constant_texture{vec3{.980f, .854f, .368f}}
	)};
	texture& ground_green{_texture_spreader.add_object(
		new constant_texture{vec3{.8f, .8f, .0f}}
	)};
	texture& checker{_texture_spreader.add_object(
		new checker_texture{solid_yellow, ground_green}
	)};
	texture& noise{_texture_spreader.add_object(
		new noise_texture{10.f}
	)};
	texture& powerful_yellow{_texture_spreader.add_object(
		new constant_texture{static_cast<constant_texture&>(solid_yellow).color * 5.f}
	)};
	texture& earth_map{_texture_spreader.add_object(
		new image_texture{"map-of-earth13.jpg"}
	)};
	texture& earth_ground{_texture_spreader.add_object(
		new image_texture{"map-of-earth20.jpg"}
	)};

	material& textured_metal{_material_spreader.add_object(
		new texture_mapped_fuzziness_metal{vec3::one(), earth_ground}
	)};
	material& default_light{_material_spreader.add_object(
		new diffuse_light{powerful_yellow}
	)};
	material& intensity_scaler{_material_spreader.add_object(
		new multiply{3.f}
	)};

	_scene.list.push_back(std::ref(_hitable_spreader.add_object<hitable>(
		new sphere{vec3{0.f, .5f, -2.5f},
		.5f,
		_material_spreader.add_object<material>(
			new lambertian{noise})
	}
	)));
	
	hitable& chiseled_metal_ball{_hitable_spreader.add_object<hitable>(
		new sphere{vec3{0.f, 0.5f, .5f},
		.5f,
		intensity_scaler
		//textured_metal
		//   material_spreader.add_object<material>(
		//	   new metal{vec3{1.f, 1.f, 1.f}, 0.f})
	})};
	_scene.list.push_back(std::ref(chiseled_metal_ball));
	_favoredHitables.list.push_back(std::ref(chiseled_metal_ball));

	_scene.list.push_back(std::ref(_hitable_spreader.add_object<hitable>(
		new sphere{vec3{.0f, -1000.f, -1.f},
		1000.f,
		_material_spreader.add_object<material>(
			new lambertian{checker}
			)
	}
	)));

	hitable& light = _hitable_spreader.add_object<hitable>(
		new flip_normals{_hitable_spreader.add_object(
			new xy_rect{-2.f, 2.f, 2.f, 6.f, 5.f, default_light}
		)}
	);
	_scene.list.push_back(std::ref(light));
	_favoredHitables.list.push_back(std::ref(light));

#ifdef HOLLOWED_GLASS_BALL
	_scene.list.push_back(std::ref(_hitable_spreader.add_object<hitable>(
		new sphere{vec3{1.1f, 0.5f, -1.f},
		.5f,
		_material_spreader.add_object<material>(
			new dielectric{1.5f})
	}
	)));
	_scene.list.push_back(std::ref(_hitable_spreader.add_object<hitable>(
		new sphere{vec3{1.1f, 0.5f, -1.f},
		-.49f,
		_material_spreader.add_object<material>(
			new dielectric{1.5f})
	}
	)));
#endif

	_scene.list.push_back(std::ref(_hitable_spreader.add_object<hitable>(
		new sphere{vec3{-1.1f, 0.5f, -1.f},
		.5f,
		_material_spreader.add_object<material>(
			new metal{vec3{.6f, .8f, .8f}, 0.3f})
	}
	)));
	_scene.list.push_back(std::ref(_hitable_spreader.add_object<hitable>(
		new moving_sphere{vec3{0.f, .75f, -1.f}, vec3{0.f, .9f, -1.f},
		.0f, 1.f,
		.5f,
		_material_spreader.add_object<material>(
			new lambertian{earth_map}
			)
	}
	)));

#ifdef SPAWN_RANDOM_SPHERES
	add_random_spheres(_scene, RANDOM_SPHERE_COUNT, vec3{5.f, 0.f, 5.f},
					   _hitable_spreader, _material_spreader, _texture_spreader);
#endif
}



vec3
color(const ray& _r, const hitable& _world, const hitable& _lightShape, int _depth)
{
	hit_record hrecord;
	scatter_record srecord;
	if (_world.hit(_r, 0.001f, std::numeric_limits<float>::max(), hrecord))
	{
#ifdef COLOR_LIT_SCENE
		vec3 emitted = hrecord.p_material->emit(_r, hrecord, hrecord.u, hrecord.v, hrecord.p);
#endif

		if (_depth < 100 && hrecord.p_material->scatter(_r, hrecord, srecord))
#ifdef COLOR_LIT_SCENE
		{
#ifndef IMPORTANCE_SAMPLING
			return emitted + srecord.attenuation * color(srecord.specular_ray, _world, _lightShape, _depth + 1);
#else
			if (srecord.is_specular)
			{
				return srecord.attenuation * color(srecord.specular_ray, _world, _lightShape, _depth+1);
			}
			else
			{
				constant_texture black{vec3{0.f, 0.f, 0.f}};
				lambertian dummy{black};
				hitable_pdf p0{_lightShape, hrecord.p};
				mixture_pdf p{p0, *srecord.p_pdf};

				ray scattered = ray{hrecord.p, p.generate(), _r.time};
				float pdf_value = p.value(scattered.direction());
				float scattering = hrecord.p_material->scattering_pdf(_r, hrecord, scattered);

				vec3 result{vec3::zero()};

				//if (pdf_value == 0.f)
				//{
				//	//std::cout << "Scattered ray probability is null." << std::endl;
				//	//result = emitted + srecord.attenuation * color(scattered, _world, _lightShape, _depth + 1);
				//	result = emitted;
				//}
				//else
				//{
					result =
						emitted +
						srecord.attenuation * scattering * color(scattered, _world, _lightShape, _depth + 1)
						/ pdf_value;
				//}

				if (pdf_value == 0.f)
					std::cout << "Scattered direction probability is null" << std::endl;
				if (!(result.e[0] == result.e[0]))
					std::cout << "NaN found on result.x()" << std::endl;
				if (!(result.e[1] == result.e[1]))
					std::cout << "NaN found on result.y()" << std::endl;
				if (!(result.e[2] == result.e[2]))
					std::cout << "NaN found on result.z()" << std::endl;

				return result;
			}
#endif
		}
#else
			return srecord.attenuation * color(srecord.specular_ray, _world, _lightShape, _depth + 1);
#endif
		else
#ifdef COLOR_LIT_SCENE
			return emitted;
#else
			return vec3::zero();
#endif
	}
	else
	{
#ifdef COLOR_LIT_SCENE
		return vec3::zero();
#else
		vec3 unit_direction = unit_vector(_r.direction());
		float t = 0.5f * (unit_direction.y() + 1.0f);
		return (1.0f - t) * vec3::one() + t * vec3{ .5f, .7f, 1.f };
#endif
	}
}



#ifdef MULTITHREADED
#include <mutex>
#include <atomic>

struct pixel_job
{
	int sample_count;
	int image_width, image_height;
	int min_x, min_y;
	int width, height;
	const camera& source_camera;
	const hitable& world;
	const hitable& favored_hitable;

	std::vector<vec3>& result_storage;

	void operator()()
	{
		for (int j = 0; j < height; ++j)
		{
			for (int i = 0; i < width; ++i)
			{
				int x = min_x + i;
				int y = min_y + j;

				vec3 col{vec3::zero()};
				float weights_sum = 0.f;
				for (int s = 0; s < sample_count; ++s)
				{
#ifdef NONUNIFORM_SAMPLING
					float xi = float(x) + g_RNG.sample();
					float yi = float(y) + g_RNG.sample();
#else
					float xi = float(x);
					float yi = float(y);
#endif

					float u = xi / float(image_width);
					float v = yi / float(image_height);


					// Filter's radius is defined as 0.5f for now.
					const float filter_radius = 0.5f;
					float ux = (float(x) + 0.5f) - xi;
					float vy = (float(y) + 0.5f) - yi;
					float filter_value;

#ifdef GAUSSIAN_FILTER
					const float alpha = .001f;
					float exponent = std::exp(-alpha * filter_radius * filter_radius);
					filter_value =
						std::max(0.f, std::exp(-alpha * ux * ux) - exponent) *
						std::max(0.f, std::exp(-alpha * vy * vy) - exponent);
#endif
#ifdef TRIANGLE_FILTER
					filter_value =
						std::max(0.f, filter_radius - std::abs(ux)) *
						std::max(0.f, filter_radius - std::abs(vy));
#endif
#ifdef BOX_FILTER
					filter_value = 1.f;
#endif

					ray r{source_camera.get_ray(u, v)};

					vec3 result{color(r, world, favored_hitable, 0)};
					col += de_nan(result) * filter_value;
					weights_sum += filter_value;
				}

				col /= weights_sum;//float(sample_count);
				col = vec3{sqrt(col.r()), sqrt(col.g()), sqrt(col.b())};
				col = vec3{ffmin(col.r(), 1.f), ffmin(col.g(), 1.f), ffmin(col.b(), 1.f)};

				result_storage[x + y * image_width] = col;
			}
		}
	}
};
std::vector<pixel_job*> pending_jobs;
std::mutex jobs_mutex;
std::atomic_int jobs_access{0};

struct worker_thread
{
	std::atomic_int& _threadCount;

	void operator()()
	{
		for (;;)
		{
			//jobs_mutex.lock();
			int jobs_access_value = 0;
			while (!jobs_access.compare_exchange_weak(jobs_access_value, 1))
				jobs_access_value = 0;

			size_t remaining_jobs = pending_jobs.size();
			if (remaining_jobs == 0)
			{
				//jobs_mutex.unlock();
				jobs_access--;
				break;
			}

			pixel_job& current_job = *pending_jobs[remaining_jobs - 1];
			pending_jobs.pop_back();

			//jobs_mutex.unlock();
			jobs_access--;

			current_job();

			delete &current_job;
		}

		_threadCount--;
	}
};
#endif





int 
main()
{
	const int image_width = IMAGE_WIDTH;
	const int image_height = IMAGE_HEIGHT;
	const int sample_count = SAMPLE_COUNT;

	perlin::startup();

	lifetime_spreader<hitable> hitable_spreader{};
	lifetime_spreader<material> material_spreader{};
	lifetime_spreader<texture> texture_spreader{};

	camera main_camera{};
	hitable_list world{};
	hitable_list favored_hitable{};


#ifdef USUAL_SCENE
	usual_scene(world, favored_hitable, hitable_spreader, material_spreader, texture_spreader);

	vec3 view_position{2.f, .4f, -1.3f};
	vec3 look_position{0.f, .75f, -1.f};
	float dist_to_focus = (view_position - look_position).length();
	float aperture = 0.f;
	main_camera = camera{
		view_position, look_position, vec3{.0f, 1.f, .0f},
		90.f, float(image_width) / float(image_height), aperture,
		dist_to_focus, 0.f, 1.f
	};
#endif

#ifdef CORNELL_BOX
	cornell_box(world, favored_hitable, hitable_spreader, material_spreader, texture_spreader);

	vec3 view_position{278.f, 278.f, -800.f};
	vec3 look_position{278.f, 278.f, 0.f};
	float dist_to_focus = 10.f;
	float aperture = 0.f;
	float fov = 40.f;
	main_camera = camera{
		view_position, look_position, vec3{0.f, 1.f, 0.f},
		fov, float(image_width) / float(image_height), aperture,
		dist_to_focus, 0.f, 1.f
	};
#endif

	bvh_node bvh_world{world.list, 0.f, 1.f, hitable_spreader};

	auto begin = std::chrono::system_clock::now();
	unsigned char* pixels = new unsigned char[image_height * image_width * 3]{};

#ifdef MULTITHREADED
	const int group_width = GROUP_WIDTH;
	const int group_height = GROUP_HEIGHT;
	int jobs_start_count;

	std::vector<vec3> hdr_buffer{image_height * image_width};
	//pending_jobs.resize(image_height * image_width);

	for (int j = 0; j < image_height; j += group_height)
	{
		int local_height = ficlamp(image_height - j, 0, group_height);

		for (int i = 0; i < image_width; i += group_width)
		{
			int local_width = ficlamp(image_width - i, 0, group_width);

			pixel_job *current_job{new pixel_job{
				sample_count,
				image_width, image_height,
				i, j,
				local_width, local_height,
				main_camera,
				bvh_world, favored_hitable,
				hdr_buffer
			}};
			//pending_jobs[j * image_width + i] = current_job;
			pending_jobs.push_back(current_job);
		}
	}
	jobs_start_count = int(pending_jobs.size());

	std::atomic_int atomic_workers_count{0};
	std::vector<std::thread> workers;
	for (int i = 0; i < 8; ++i)
	{
		atomic_workers_count++;
		workers.push_back(std::thread{worker_thread{atomic_workers_count}});
	}

	int workers_count{atomic_workers_count.load()};
	int progress = 0;
	while (workers_count > 0)
	{
		workers_count = atomic_workers_count.load();

		jobs_mutex.lock();
		size_t remaining_jobs = pending_jobs.size();
		jobs_mutex.unlock();
		int current_progress = int(100.f * (1.f - float(remaining_jobs) / jobs_start_count));// (image_width * image_height)));

		while (current_progress > progress)
		{
			progress++;
			std::cout << progress << " %" << std::endl;
		}
	}

	for (std::thread& worker : workers)
	{
		worker.join();
	}

	unsigned char* head = pixels;
	for (int j = image_height - 1; j >= 0; j--)
	{
		for (int i = 0; i < image_width; i++)
		{
			vec3 col{hdr_buffer[j * image_width + i]};

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
	}

#else
	unsigned char* head = pixels;
	for (int j = image_height - 1; j >= 0; j--)
	{
		for (int i = 0; i < image_width; i++)
		{
			vec3 col{0.f, 0.f, 0.f};
			for (int s = 0; s < sample_count; ++s)
			{
				float u = (float(i) + g_RNG.sample()) / float(image_width);
				float v = (float(j) + g_RNG.sample()) / float(image_height);

				ray r{main_camera.get_ray(u, v)};

				vec3 result{color(r, bvh_world, favored_hitable, 0)};
				col += de_nan(result);
			}

			col /= float(sample_count);
			col = vec3{sqrt(col.r()), sqrt(col.g()), sqrt(col.b())};
			col = vec3{ffmin(col.r(), 1.f), ffmin(col.g(), 1.f), ffmin(col.b(), 1.f)};

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
#endif // ifdef MULTITHREADED

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

	getchar();
}
