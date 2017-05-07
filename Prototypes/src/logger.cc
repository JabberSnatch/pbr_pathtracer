#include "logger.hpp"

#include <ctime>


const std::string CentralLogger::paths[] = { "general.log", "geometry.log", "memory.log" };

LocalLogger::LocalLogger(CentralLogger &_central) :
	central{ _central }
{
	for (int i = 0; i < kLogChannelCount; ++i)
		message_buffers[i].reserve(buffer_size);
}
void
LocalLogger::Log(LogChannel _channel, LogLevel _level, std::string const &_message)
{
	log_locks[_channel].Acquire();
	message_buffers[_channel].emplace_back(LogMessage(_channel, _level, _message));
	log_locks[_channel].Release();

	if (message_buffers[_channel].size() >= buffer_size && !central.flush_locks[_channel].is_locked())
	{
		central.MergeMessages(_channel, message_buffers[_channel]);
		message_buffers[_channel].erase(message_buffers[_channel].begin(), message_buffers[_channel].end());
	}
}

CentralLogger::CentralLogger()
{
	for (int i = 0; i < kLogChannelCount; ++i)
		file_streams[i].open(paths[i]);
}
CentralLogger::~CentralLogger()
{
	for (int i = 0; i < kLogChannelCount; ++i)
	{
		for (int j = 0; j < thread_count; ++j)
			MergeMessages(LogChannel(i), local_loggers[j].message_buffers[i]);
		FlushChannel(LogChannel(i));
	}
}

void
CentralLogger::SetThreadCount(size_t _thread_count)
{
	thread_count = _thread_count;
	local_loggers.clear();
	local_loggers.reserve(thread_count);
	for (size_t i = 0; i < _thread_count; ++i)
		local_loggers.emplace_back(LocalLogger(*this));
}

void
CentralLogger::MergeMessages(LogChannel _channel, std::vector<LogMessage> &_messages)
{
	merge_locks[_channel].Acquire();

	for (auto &&message : _messages)
		message_history[_channel].emplace(message.date, std::move(message));

	if (message_history[_channel].size() >= buffer_size && !flush_locks[_channel].is_locked())
		FlushChannel(_channel, true);
	else
		merge_locks[_channel].Release();
}

void
CentralLogger::FlushChannel(LogChannel _channel, bool _release_merge_lock)
{
	flush_locks[_channel].Acquire();

	for (int i = 0; i < thread_count; ++i)
	{
		local_loggers[i].log_locks[_channel].Acquire();
	}
	for (int i = 0; i < thread_count; ++i)
	{
		for (auto &&message : local_loggers[i].message_buffers[_channel])
			message_history[_channel].emplace(message.date, std::move(message));
		local_loggers[i].message_buffers[_channel].erase(
			local_loggers[i].message_buffers[_channel].begin(),
			local_loggers[i].message_buffers[_channel].end());
	
		local_loggers[i].log_locks[_channel].Release();
	}

	std::map<LogMessage::TimeStamp_t, LogMessage>	history_copy = message_history[_channel];
	message_history[_channel].clear();

	write_locks[_channel].Acquire();
	if (_release_merge_lock)
		merge_locks[_channel].Release();

	int pair_index = 0;
	for (auto &&pair : history_copy)
	{
		file_streams[_channel] << pair_index++ << " ";
		StreamMessage(file_streams[_channel], pair.second);
	}

	write_locks[_channel].Release();
	flush_locks[_channel].Release();
}

LocalLogger&
CentralLogger::operator[](uint32_t _index)
{
	// ASSERT _index validity
	return local_loggers[_index];
}

void
CentralLogger::StreamMessage(std::ostream &_stream, LogMessage const &_message)
{
	_stream << _message.level << " : " << std::to_string(_message.date.count()) << ", " << _message.message << std::endl;
}