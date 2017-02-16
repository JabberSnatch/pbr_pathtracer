#include "random.hpp"

//#define FINITE_COUNT
//#define INFINITE_RUN
//#define STRATIFIED
#define IMPORTANCE_SAMPLING


inline float
pdf(float _x)
{
	return 3.f*_x*_x / 8.f;
}

inline float
cdf(float _x)
{
	return _x*_x*_x / 8.f;
}

inline float
cdf_inv(float _n)
{
	return powf(8.f * _n, 1.f/ 3.f);
}


int
main()
{
	int inside_circle = 0;

#ifdef FINITE_COUNT
	const int N = 1000000;
	for (int i = 0; i < N; ++i)
	{
		float x = random::sample_relative();
		float y = random::sample_relative();
		if (x*x + y*y < 1.f)
			++inside_circle;
	}
	std::cout << "Estimate of pi : " << 4.f * float(inside_circle) / N << std::endl;
#endif

#ifdef INFINITE_RUN
	unsigned int runs = 0;
	for (;;)
	{
		++runs;
		float x = random::sample_relative();
		float y = random::sample_relative();
		if (x*x + y*y < 1.f)
			++inside_circle;
		if ((runs & 0xfffffu) == 0)
			std::cout << "Estimate of pi : " << 4.f * float(inside_circle) / runs << " (" << runs << ")" << std::endl;
	}
#endif
	
#ifdef STRATIFIED
	const int sqrt_N = 10000;
	int inside_circle_stratified = 0;

	for (int i = 0; i < sqrt_N; ++i)
	{
		for (int j = 0; j < sqrt_N; ++j)
		{
			float x = random::sample_relative();
			float y = random::sample_relative();
			if (x*x + y*y < 1.f)
				++inside_circle;
			x = 2.f * ((i + random::sample()) / sqrt_N) - 1.f;
			y = 2.f * ((j + random::sample()) / sqrt_N) - 1.f;
			if (x*x + y*y < 1.f)
				++inside_circle_stratified;
		}
	}

	std::cout << "Regular    Estimate of pi : " << 4.f * float(inside_circle) / (sqrt_N*sqrt_N) << std::endl;
	std::cout << "Stratified Estimate of pi : " << 4.f * float(inside_circle_stratified) / (sqrt_N*sqrt_N) << std::endl;
#endif

#ifdef IMPORTANCE_SAMPLING
	const int N = 1;
	float sum = .0f;
	for (int i = 0; i < N; ++i)
	{
		float x = cdf_inv(random::sample());
		sum += x*x / pdf(x);
	}
	std::cout << "I = " << sum / N << std::endl;
#endif

	getchar();
}

