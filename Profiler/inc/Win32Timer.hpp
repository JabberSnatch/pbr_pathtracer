#ifndef __YS_WIN32_TIMER_HPP__
#define __YS_WIN32_TIMER_HPP__

#include <Windows.h>


struct MasterTimer
{
	using Duration = long long;
	using Period = LARGE_INTEGER;

	inline void
	add(Duration const &_time)
	{
		accumulated_time += _time;
		if (_time < best_time)
			best_time = _time;
		++call_count;
	}

	inline double
	total() const { return accumulated_time / (double)frequency.QuadPart; }
	inline double
	average() const { return total() / (double)call_count; }
	inline double
	best() const { return best_time / (double)frequency.QuadPart; }
	inline Duration
	total_tick() const { return accumulated_time; }
	inline Duration
	best_tick() const { return best_time; }
	inline Duration
	average_tick() const { return accumulated_time / call_count; }

	char const			*name {};
	Duration			accumulated_time {0};
	Duration			best_time {_I64_MAX};
	unsigned long long	call_count {0};
	

	static inline Period
	Frequency()
	{
		Period frequency;
		QueryPerformanceFrequency(&frequency);
		return frequency;
	}
	static Period		frequency;
};
MasterTimer::Period MasterTimer::frequency {MasterTimer::Frequency()};


struct SlaveTimer
{
	using TimePoint = LARGE_INTEGER;

	~SlaveTimer()
	{
		TimePoint end {Now()};
		master.add(end.QuadPart - start.QuadPart);
	}

	MasterTimer		&master;
	TimePoint		start {Now()};


	static inline TimePoint 
	Now() 
	{ 
		TimePoint now; 
		QueryPerformanceCounter(&now);
		return now;
	}
};


#endif // __YS_WIN32_TIMER_HPP__
