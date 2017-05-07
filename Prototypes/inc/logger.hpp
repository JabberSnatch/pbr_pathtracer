#pragma once
#ifndef __YS_PROTO_LOGGER_HPP__
#define __YS_PROTO_LOGGER_HPP__

#include <chrono>
#include <string>
#include <vector>
#include <map>
#include <atomic>
#include <fstream>

enum LogChannel { kLogGeneral = 0, kLogGeometry, kLogMemory, kLogChannelCount, kLogStdOut };
enum LogLevel { kLogDebug = 0, kLogInfo, kLogWarning, kLogError, kLogLevelCount };

namespace globals {
static std::chrono::high_resolution_clock::time_point	start = std::chrono::high_resolution_clock::now();
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
	LogMessage(LogChannel _channel, LogLevel _level, std::string const &_message) :
		channel{ _channel }, level{ _level }, message{ _message },
		date{ Clock_t::now() - globals::start }
	{}

	LogChannel	channel;
	LogLevel	level;
	std::string	message;
	TimeStamp_t	date;
};

struct CentralLogger;

struct LocalLogger final
{
	LocalLogger(CentralLogger &_central);
	void Log(LogChannel _channel, LogLevel _level, std::string const &_message);

	static constexpr size_t		buffer_size = 32;
	CentralLogger				&central;
	std::vector<LogMessage>		message_buffers[LogChannel::kLogChannelCount];
	uint32_t					message_counts[LogChannel::kLogChannelCount];
	SpinLock					log_locks[LogChannel::kLogChannelCount];
};

struct CentralLogger final
{
	CentralLogger();
	~CentralLogger();

	void SetThreadCount(size_t _thread_count);
	void MergeMessages(LogChannel _channel, std::vector<LogMessage> &_messages);
	void FlushChannel(LogChannel _channel, bool _release_merge_lock = false);
	LocalLogger &operator[](uint32_t _index);
	void StreamMessage(std::ostream &_stream, LogMessage const &_message);

	static constexpr size_t							buffer_size = 256;
	static const std::string						paths[];
	size_t											thread_count;
	std::vector<LocalLogger>						local_loggers;
	std::map<LogMessage::TimeStamp_t, LogMessage>	message_history[LogChannel::kLogChannelCount];
	SpinLock										merge_locks[LogChannel::kLogChannelCount];
	SpinLock										flush_locks[LogChannel::kLogChannelCount];
	SpinLock										write_locks[LogChannel::kLogChannelCount];
	std::ofstream									file_streams[LogChannel::kLogChannelCount];
};


#endif // __YS_PROTO_LOGGER_HPP__
