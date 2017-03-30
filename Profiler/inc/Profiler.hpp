#ifndef __YS_PROFILER_HPP__
#define __YS_PROFILER_HPP__

#include <chrono>
#include <unordered_map>

#include "Timer.hpp"


struct Profiler
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
		static constexpr u64 prime {1099511628211Ui64};
		static constexpr u64 offset {0xcbf29ce484222325Ui64};

		static constexpr u64
		compute_hash(char const *_v)
		{
			u64 hash {offset};
			while (*_v != '\0')
			{
				hash *= prime;
				hash ^= *_v;
				++_v;
			}
			return hash;
		}

		constexpr FNV_Hash(char const *_v)
			: hash {compute_hash(_v)}, value {_v}
		{}

		u64 const hash;
		char const *const value;
	};

	using TimerTable = std::unordered_map<u64, MasterTimer, IdentityHash<u64>>;

	static MasterTimer&
	GetMaster(FNV_Hash const &_hash)
	{
		auto ite{timers.find(_hash.hash)};
		if (ite != timers.end())
			return ite->second;
		return timers.emplace(_hash.hash, MasterTimer {_hash.value}).first->second;
	}

	static TimerTable timers;
};


Profiler::TimerTable Profiler::timers{};


//constexpr Profiler::u64
//operator "" _hashed(char const *_key, size_t _size)
//{
//	return Profiler::FNV_Hash {_key}.hash;
//}


#endif // __YS_PROFILER_HPP__
