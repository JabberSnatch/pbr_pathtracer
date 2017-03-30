
#ifdef _WIN32
#define WIN32_TIMERS
#endif

#include <intrin.h>

#include <vector>

#include "random.hpp"

#include "Profiler.hpp"
#include "NaiveProfiler.hpp"


#define BENCH_COUNT 5
#define ITER_COUNT 10000
#define VALUE_COUNT 10000

#define PROFILERS_BENCH
//#define SORT_BENCH


void	bubble_sort(std::vector<float> &_vector);
void	qsort(std::vector<float> &_vector);
void	qsort_helper(std::vector<float> &_vector, size_t _begin, size_t _end);
size_t	partition(std::vector<float> &_vector, size_t _begin, size_t _end);


void	profilers_benchmark(size_t const _iter_count, size_t const _querry_count);
void	sort_benchmark(size_t const _iter_count, size_t const _array_size);


void	print_timer(MasterTimer const &_timer);


int
main()
{
	size_t const bench_count {BENCH_COUNT};
	size_t const iter_count {ITER_COUNT};
	size_t const value_count {VALUE_COUNT};

	for (int i = 0; i < bench_count; ++i)
	{
#ifdef PROFILERS_BENCH
		profilers_benchmark(iter_count, value_count);
#endif // GET_MASTER_BENCH

#ifdef SORT_BENCH
		sort_benchmark(iter_count, value_count);
#endif // SORT_BENCH
	}

	std::getchar();
	return 0;
}


void
bubble_sort(std::vector<float> &_vector)
{
	SlaveTimer bubble_timer{Profiler::GetMaster("BUBBLE")};

	for (;;)
	{
		SlaveTimer inner_timer{Profiler::GetMaster("INNER")};
		bool sorted = true;

		for (int j = 0; j < _vector.size() - 1; ++j)
		{
			float &a = _vector[j];
			float &b = _vector[j + 1];
			if (a > b)
			{
				std::swap(a, b);
				sorted = false;
			}
		}

		if (sorted)
			break;
	}
}


void 
qsort(std::vector<float> &_vector)
{
	SlaveTimer timer {Profiler::GetMaster("QSORT")};

	qsort_helper(_vector, 0, _vector.size() - 1);
}


void 
qsort_helper(std::vector<float> &_vector, size_t _begin, size_t _end)
{
	// NOTE: Profiling a recursive function yields "wrong" results because it
	//		 counts recursive calls as separate instances of the function.
	//		 The result might be used to find out how much time is spent
	//		 recursing when compared with a proper timing of a non-recursive
	//		 interface function (qsort in this instance).
	SlaveTimer timer {Profiler::GetMaster("QSORT_HELPER")};

	if (_begin < _end)
	{
		size_t pivot {partition(_vector, _begin, _end)};
		qsort_helper(_vector, _begin, pivot - 1);
		qsort_helper(_vector, pivot + 1, _end);
	}
}


size_t
partition(std::vector<float> &_vector, size_t _begin, size_t _end)
{
	SlaveTimer timer {Profiler::GetMaster("QSORT_PARTITION")};

	float pivot {_vector[_end]};
	size_t i {_begin};
	for (size_t j {_begin}; j < _end; ++j)
	{
		if (_vector[j] <= pivot)
		{
			std::swap(_vector[i], _vector[j]);
			++i;
		}
	}
	std::swap(_vector[i + 1], _vector[_end]);
	return i + 1;
}


void
profilers_benchmark(size_t const _iter_count, size_t const _querry_count)
{
	NaiveProfiler::timers.clear();
	FNV_Hashed_Profiler::timers.clear();
	FNV_PreHashed_Profiler::timers.clear();
	Profiler::timers.clear();


	std::cout << std::endl << "PROFILERS_BENCH" << std::endl;

	std::cout << "Profiling " << _iter_count << " iterations of " << _querry_count << " calls to [x]Profiler::GetMaster" << std::endl;

	// NAIVE IMPLEMENTATION
	std::cout << std::endl << "BASELINE" << std::endl;
	{
		for (size_t iter = 0; iter < _iter_count; ++iter)
		{
			SlaveTimer timer {NaiveProfiler::GetMaster("GET_MASTER")};
			for (size_t querry = 0; querry < _querry_count; ++querry)
				NaiveProfiler::GetMaster("DUMMY");
		}

		long long cycle_count = 0;
		long long start;

		start = __rdtsc();
		{
			SlaveTimer timer{ NaiveProfiler::GetMaster("GET_MASTER") };
		}
		cycle_count += __rdtsc() - start;

		std::cout << cycle_count << std::endl;

		for (auto& pair : NaiveProfiler::timers)
			print_timer(pair.second);
	}

	// FNV_HASH IMPLEMENTATION
	std::cout << std::endl << "FNV_HASHED" << std::endl;
	{
		for (size_t iter = 0; iter < _iter_count; ++iter)
		{
			SlaveTimer timer {FNV_Hashed_Profiler::GetMaster("GET_MASTER")};
			for (size_t querry = 0; querry < _querry_count; ++querry)
				FNV_Hashed_Profiler::GetMaster("DUMMY");
		}

		long long cycle_count = 0;
		long long start;

		start = __rdtsc();
		{
			SlaveTimer timer{ FNV_Hashed_Profiler::GetMaster("GET_MASTER") };
		}
		cycle_count += __rdtsc() - start;

		std::cout << cycle_count << std::endl;

		for (auto& pair : FNV_Hashed_Profiler::timers)
			print_timer(pair.second);
	}

	// CONSTEXPR IMPLEMENTATION
	std::cout << std::endl << "PRE-HASHED (I hope?)" << std::endl;
	{
		for (size_t iter = 0; iter < _iter_count; ++iter)
		{
			SlaveTimer timer {FNV_PreHashed_Profiler::GetMaster("GET_MASTER")};
			for (size_t querry = 0; querry < _querry_count; ++querry)
				FNV_PreHashed_Profiler::GetMaster("DUMMY");
		}

		long long cycle_count = 0;
		long long start;

		start = __rdtsc();
		{
			SlaveTimer timer{ FNV_PreHashed_Profiler::GetMaster("GET_MASTER") };
		}
		cycle_count += __rdtsc() - start;

		std::cout << cycle_count << std::endl;

		for (auto& pair : FNV_PreHashed_Profiler::timers)
			print_timer(pair.second);
	}

	FNV_PreHashed_Profiler::timers.clear();
	// CONSTEXPR IMPLEMENTATION (Using user-defined literal)
	std::cout << std::endl << "PRE-HASHED (I hope?) + user-defined literal" << std::endl;
	{
		for (size_t iter = 0; iter < _iter_count; ++iter)
		{
			SlaveTimer timer {FNV_PreHashed_Profiler::GetMaster("GET_MASTER"_hashed, "GET_MASTER")};
			for (size_t querry = 0; querry < _querry_count; ++querry)
				FNV_PreHashed_Profiler::GetMaster("DUMMY"_hashed, "DUMMY");
		}

		long long cycle_count = 0;
		long long start;

		start = __rdtsc();
		{
			SlaveTimer timer{ FNV_PreHashed_Profiler::GetMaster("GET_MASTER"_hashed, "GET_MASTER") };
		}
		cycle_count += __rdtsc() - start;

		std::cout << cycle_count << std::endl;

		for (auto& pair : FNV_PreHashed_Profiler::timers)
			print_timer(pair.second);
	}

	// EXTENDED CONSTEXPR IMPLEMENTATION
	std::cout << std::endl << "LATEST" << std::endl;
	{
		for (size_t iter = 0; iter < _iter_count; ++iter)
		{
			SlaveTimer timer {Profiler::GetMaster(Profiler::FNV_Hash{"GET_MASTER"})};
			for (size_t querry = 0; querry < _querry_count; ++querry)
				Profiler::GetMaster(Profiler::FNV_Hash {"DUMMY"});
		}

		long long cycle_count = 0;
		long long start;

		start = __rdtsc();
		{
			SlaveTimer timer{ Profiler::GetMaster(Profiler::FNV_Hash{ "GET_MASTER" }) };
		}
		cycle_count += __rdtsc() - start;

		std::cout << cycle_count << std::endl;

		for (auto& pair : Profiler::timers)
			print_timer(pair.second);
	}
}


void
sort_benchmark(size_t const _iter_count, size_t const _array_size)
{
	Profiler::timers.clear();

	std::cout << std::endl << "SORT_BENCH" << std::endl;

	std::cout << "Profiling " << _iter_count << " iterations over " << _array_size << " elements arrays. \n\n";

	// CONSTEXPR IMPLEMENTATION
	{
		for (size_t iter = 0; iter < _iter_count; ++iter)
		{
			std::vector<float> bubble_sort_vector {};
			std::vector<float> qsort_vector {};
			
			{
				SlaveTimer init_timer {Profiler::GetMaster("INIT")};
				
				bubble_sort_vector.resize(_array_size);
				for (size_t i = 0; i < _array_size; ++i)
					bubble_sort_vector[i] = g_RNG.sample();

				qsort_vector = bubble_sort_vector;
			}

			bubble_sort(bubble_sort_vector);
			qsort(qsort_vector);
		}

		for (auto& pair : Profiler::timers)
			print_timer(pair.second);
	}
}


void
print_timer(MasterTimer const &_timer)
{
	if (_timer.call_count == 0) return;

	std::cout << 
		_timer.name << ", " << 
		_timer.total() << ", " << 
		_timer.average() << ", " << 
		_timer.best() << ", " << 
		_timer.best_tick() << ", " <<
		_timer.total_tick() << ", " << 
		_timer.average_tick() << 
	std::endl;
}

