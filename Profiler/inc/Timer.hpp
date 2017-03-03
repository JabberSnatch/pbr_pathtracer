#ifdef WIN32_TIMERS
#include "Win32Timer.hpp"
#else

#ifndef __YS_TIMER_HPP__
#define __YS_TIMER_HPP__

#include <chrono>
#include <string>


struct MasterTimer
{
	using ClockType = std::chrono::high_resolution_clock;
	using Duration = ClockType::duration;
	using Period = ClockType::period;

	inline void 
	add(Duration const &_time)
	{
		accumulated_time += _time;
		if (_time < best_time)
			best_time = _time;
		++call_count;
	}

	inline double
	total() const { return accumulated_time.count() / (double)Period::den; }
	inline double 
	average() const { return total() / (double)call_count; }
	inline double 
	best() const { return best_time.count() / (double)Period::den; }
	inline unsigned long long
	total_tick() const { return accumulated_time.count(); }
	inline unsigned long long
	best_tick() const { return best_time.count(); }
	inline unsigned long long
	average_tick() const { return accumulated_time.count() / call_count; }

	char const			*name {};
	Duration			accumulated_time {0};
	Duration			best_time {Duration::max()};
	unsigned long long	call_count {0};
};


struct SlaveTimer
{
	using ClockType = std::chrono::high_resolution_clock;

	~SlaveTimer()
	{
		ClockType::time_point end{ClockType::now()};
		master.add(end - start);
	}

	MasterTimer				&master;
	ClockType::time_point	start{ClockType::now()};
};


#endif // __YS_TIMER_HPP__

#endif // WIN32_TIMER
