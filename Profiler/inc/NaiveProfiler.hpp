#ifndef __YS_NAIVE_PROFILER_HPP__
#define __YS_NAIVE_PROFILER_HPP__

#include "Timer.hpp"


struct NaiveProfiler
{
	using TimerTable = std::unordered_map<char const *, MasterTimer>;

	static MasterTimer& GetMaster(char const *_key)
	{
		auto ite{timers.find(_key)};
		if (ite != timers.end())
			return ite->second;
		return timers.emplace(_key, MasterTimer {_key}).first->second;
	}

	static TimerTable timers;
};

NaiveProfiler::TimerTable NaiveProfiler::timers {};


struct FNV_Hashed_Profiler
{
	using u64 = unsigned long long;

	struct FNV_Hash
	{
		const u64 prime {1099511628211Ui64};
		const u64 offset {0xcbf29ce484222325Ui64};

		u64 operator()(char const *_v) const
		{
			u64 hash {offset};
			char const *ite {_v};
			while (*ite != '\0')
			{
				hash *= prime;
				hash ^= *ite;
				++ite;
			}
			return hash;
		}
	};

	using TimerTable = std::unordered_map<char const *, MasterTimer, FNV_Hash>;

	static MasterTimer& GetMaster(char const *_key)
	{
		auto ite {timers.find(_key)};
		if (ite != timers.end())
			return ite->second;
		return timers.emplace(_key, MasterTimer {_key}).first->second;
	}

	static TimerTable timers;
};

FNV_Hashed_Profiler::TimerTable FNV_Hashed_Profiler::timers {};


#endif // __YS_NAIVE_PROFILER_HPP__
