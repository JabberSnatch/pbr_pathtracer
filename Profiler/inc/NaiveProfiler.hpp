#ifndef __YS_NAIVE_PROFILER_HPP__
#define __YS_NAIVE_PROFILER_HPP__

#include <unordered_map>
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


struct FNV_PreHashed_Profiler
{
	using u64 = unsigned long long;

	template<typename T>
	struct IdentityHash
	{
		constexpr IdentityHash() = default;
		constexpr T
		operator()(T const &_v) const { return _v; }
	};

	struct FNV_Hash
	{
		constexpr FNV_Hash() = default;

		static constexpr u64 prime {1099511628211Ui64};
		static constexpr u64 offset {0xcbf29ce484222325Ui64};

		constexpr u64
		operator()(char const *_v) const
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

	using TimerTable = std::unordered_map<u64, MasterTimer, IdentityHash<u64>>;

	static constexpr u64
	compute_hash(char const *_key)
	{
		return FNV_Hash {}(_key);
	}

	static MasterTimer&
	GetMaster(char const *_key)
	{
		u64 hash {compute_hash(_key)};
		auto ite {timers.find(hash)};
		if (ite != timers.end())
			return ite->second;
		return timers.emplace(hash, MasterTimer {_key}).first->second;
	}

	static MasterTimer&
	GetMaster(u64 _hash, char const *_key)
	{
		auto ite {timers.find(_hash)};
		if (ite != timers.end())
			return ite->second;
		return timers.emplace(_hash, MasterTimer {_key}).first->second;
	}

	static TimerTable timers;
};

FNV_PreHashed_Profiler::TimerTable FNV_PreHashed_Profiler::timers {};


constexpr FNV_PreHashed_Profiler::u64
operator "" _hashed(char const *_key, size_t _size)
{
	return FNV_PreHashed_Profiler::FNV_Hash {}(_key);
}


#endif // __YS_NAIVE_PROFILER_HPP__
