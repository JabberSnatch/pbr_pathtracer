#pragma once
#ifndef __YS_PROTO_LOGGER_HPP__
#define __YS_PROTO_LOGGER_HPP__

#include <chrono>
#include <string>
#include <vector>
//#include <map>
#include <set>
#include <atomic>
#include <fstream>

//#define VERSION_1
#define VERSION_2

enum LogChannel { kLogGeneral = 0, kLogGeometry, kLogMemory, kLogChannelCount, kLogStdOut };
enum LogLevel { kLogDebug = 0, kLogInfo, kLogWarning, kLogError, kLogLevelCount };

namespace globals {
static std::chrono::high_resolution_clock::time_point	start = std::chrono::high_resolution_clock::now();
extern thread_local uint32_t thread_index;
} // namespace globals

struct SpinLock final
{
	SpinLock() = default;
	// NOTE: This is not right, we have to find another way to give spinlocks to local loggers
	SpinLock(SpinLock const &_other) :
		atomic{ _other.atomic.load() }
	{}
	void Acquire()
	{
		uint32_t value = 1u;
		while (value == 1u)
			value = atomic.exchange(1u);
	}
	void Wait()
	{
		uint32_t value = 1u;
		while (value == 1u)
			value = atomic.load();
	}
	void Release()
	{
		atomic.exchange(0u);
	}
	bool is_locked()
	{
		return atomic.load() > 0u;
	}
	std::atomic_uint32_t	atomic;
};

struct LogMessage final
{
	using Clock_t = std::chrono::high_resolution_clock;
	using TimeStamp_t = Clock_t::duration;

	LogMessage() = default;
	LogMessage(LogChannel _channel, LogLevel _level, std::string &_message) :
		channel{ _channel }, level{ _level }, message{ _message },
		date{ Clock_t::now() - globals::start }
	{}

	inline bool operator<(LogMessage const &_other) const { return date < _other.date; }

	LogChannel	channel;
	LogLevel	level;
	std::string	message;
	TimeStamp_t	date;
};

struct CentralLogger;

struct LocalLogger final
{
	LocalLogger(CentralLogger &_central);
	void Log(LogChannel _channel, LogLevel _level, std::string &_message);

	static constexpr size_t		buffer_max_size = 16;
	CentralLogger				&central;
	std::vector<LogMessage>		message_buffers[LogChannel::kLogChannelCount];
#ifdef VERSION_2
	SpinLock					channel_locks[LogChannel::kLogChannelCount];
#elif defined VERSION_1
	SpinLock					buffer_locks[LogChannel::kLogChannelCount];
#else
	std::vector<LogMessage>		message_merge_buffer[LogChannel::kLogChannelCount];
	SpinLock					log_locks[LogChannel::kLogChannelCount];
#endif
};

struct CentralLogger final
{
	CentralLogger();
	~CentralLogger();

	void SetThreadCount(size_t _thread_count);

#ifdef VERSION_2
	void Merge(LogChannel _channel, std::vector<LogMessage> &_message);
	void Flush(LogChannel _channel);
	void MergeAll(LogChannel _channel);
#elif defined VERSION_1
	void Merge(LogChannel _channel, std::vector<LogMessage> &_messages);
	void Flush(LogChannel _channel, bool _release_merge_lock = false);
#else
	void Merge(LogChannel _channel, std::vector<LogMessage> &_messages);
	void Flush(LogChannel _channel);
	void MergeAll(LogChannel _channel);
#endif
	LocalLogger &operator[](uint32_t _index);
	void StreamMessage(std::ostream &_stream, LogMessage const &_message);

	static constexpr size_t							buffer_max_size = 128;
	static const std::string						paths[];
	size_t											thread_count;
	std::vector<LocalLogger>						local_loggers;
	std::multiset<LogMessage>						message_history[LogChannel::kLogChannelCount];
#ifdef VERSION_2
	SpinLock										history_locks[LogChannel::kLogChannelCount];
	//SpinLock										merge_locks[LogChannel::kLogChannelCount];
	SpinLock										flush_locks[LogChannel::kLogChannelCount];
#elif defined VERSION_1
	SpinLock										merge_locks[LogChannel::kLogChannelCount];
	SpinLock										flush_locks[LogChannel::kLogChannelCount];
	SpinLock										write_locks[LogChannel::kLogChannelCount];
#else
	SpinLock				merge_locks[LogChannel::kLogChannelCount];
	SpinLock				flush_locks[LogChannel::kLogChannelCount];
	SpinLock				log_locks[LogChannel::kLogChannelCount];
	SpinLock				flush_barriers[LogChannel::kLogChannelCount];
	SpinLock				history_locks[LogChannel::kLogChannelCount];
#endif
	std::ofstream									file_streams[LogChannel::kLogChannelCount];

	SpinLock	info_message_count_lock;
	uint32_t	info_message_count[LogChannel::kLogChannelCount] = { 0, 0, 0 };
};


#endif // __YS_PROTO_LOGGER_HPP__
