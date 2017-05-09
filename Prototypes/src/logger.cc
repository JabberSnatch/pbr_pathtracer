#include "logger.hpp"

#include <ctime>


namespace globals {
thread_local uint32_t thread_index;
}

#ifdef VERSION_1
const std::string CentralLogger::paths[] = { "general1.log", "geometry.log", "memory.log" };
#elif defined VERSION_2
const std::string CentralLogger::paths[] = { "general2.log", "geometry.log", "memory.log" };
#else
const std::string CentralLogger::paths[] = { "general3.log", "geometry.log", "memory.log" };
#endif

LocalLogger::LocalLogger(CentralLogger &_central) :
	central{ _central }
{
	for (int i = 0; i < kLogChannelCount; ++i)
		message_buffers[i].reserve(buffer_max_size);
}
void
LocalLogger::Log(LogChannel _channel, LogLevel _level, std::string &_message)
{
#ifdef VERSION_1
	central.flush_locks[_channel].Wait();

	buffer_locks[_channel].Acquire();
	message_buffers[_channel].emplace_back(LogMessage(_channel, _level, _message + " " + std::to_string(message_buffers[_channel].size() + 1)));
	size_t buffer_size = message_buffers[_channel].size();
	buffer_locks[_channel].Release();

	if (buffer_size >= buffer_max_size && !central.flush_locks[_channel].is_locked())
		central.Merge(_channel, message_buffers[_channel]);
#elif defined VERSION_2
	central.flush_locks[_channel].Wait();

	channel_locks[_channel].Acquire();
	message_buffers[_channel].emplace_back(LogMessage(_channel, _level, _message + " " + std::to_string(message_buffers[_channel].size() + 1)));
	size_t buffer_size = message_buffers[_channel].size();
	channel_locks[_channel].Release();

	if (buffer_size >= buffer_max_size && !central.flush_locks[_channel].is_locked())
	{
		central.Merge(_channel, message_buffers[_channel]);
		/*
		central.history_locks[_channel].Acquire();
		size_t history_size = central.message_history[_channel].size();
		central.history_locks[_channel].Release();
		if (!central.flush_locks[_channel].is_locked())
		{
			if (history_size + buffer_size >= central.buffer_max_size)
				central.Flush(_channel);
			else
				central.Merge(_channel, message_buffers[_channel]);
		}
		*/
	}
#else
	central.flush_barriers[_channel].Wait();
	log_locks[_channel].Acquire();

	message_buffers[_channel].emplace_back(LogMessage(_channel, _level, _message + " " + std::to_string(message_buffers[_channel].size() + 1)));
	size_t buffer_size = message_buffers[_channel].size();

	if (buffer_size >= buffer_max_size)
	{
		//std::vector<LogMessage>	buffer_copy = message_buffers[_channel];
		message_merge_buffer[_channel] = message_buffers[_channel];
		message_buffers[_channel].erase(message_buffers[_channel].begin(), message_buffers[_channel].end());
		log_locks[_channel].Release();

		central.Merge(_channel, message_merge_buffer[_channel]);
	}
	else
		log_locks[_channel].Release();
#endif
	/*
	log_locks[_channel].Acquire();
	size_t buffer_size = message_buffers[_channel].size();
	log_locks[_channel].Release();

	if (buffer_size >= buffer_max_size)
	{
		if (!central.flush_locks[_channel].is_locked())
			central.MergeMessages(_channel, message_buffers[_channel]);
		else
			// Wait on something. It could be on the flush_lock, or we could provide a CV to be
			// set whenever the message_buffer is emptied.
			central.flush_locks[_channel].Wait();
	}

	log_locks[_channel].Acquire();
	if (message_buffers[_channel].size() >= buffer_max_size)
		int i = 0;
	message_buffers[_channel].emplace_back(LogMessage(_channel, _level, _message + " " + std::to_string(message_buffers[_channel].size() + 1)));
	log_locks[_channel].Release();
	*/
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
#ifdef VERSION_1
		//for (int j = 0; j < thread_count; ++j)
		//	MergeMessages(LogChannel(i), local_loggers[j].message_buffers[i]);
		Flush(LogChannel(i));
#elif defined VERSION_2
		Flush(LogChannel(i));
#else
		Flush(LogChannel(i));
#endif
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
#ifdef VERSION_2
void
CentralLogger::Merge(LogChannel _channel, std::vector<LogMessage> &_messages)
{
	//merge_locks[_channel].Acquire();

	history_locks[_channel].Acquire();
	//message_history[_channel].emplace(LogMessage{ _channel, LogLevel::kLogDebug, "Thread " + std::to_string(globals::thread_index) + " started merging" });

	local_loggers[globals::thread_index].channel_locks[_channel].Acquire();
	for (auto &&message : _messages)
		message_history[_channel].emplace(std::move(message));

	size_t history_size = message_history[_channel].size();
	history_locks[_channel].Release();
	
	_messages.erase(_messages.begin(), _messages.end());
	local_loggers[globals::thread_index].channel_locks[_channel].Release();

	//merge_locks[_channel].Release();
	if (history_size >= buffer_max_size)
		Flush(_channel);
	
	//history_locks[_channel].Acquire();
	//message_history[_channel].emplace(LogMessage{ _channel, LogLevel::kLogDebug, "Thread " + std::to_string(globals::thread_index) + " merged to central" });
	//history_locks[_channel].Release();
}

void
CentralLogger::Flush(LogChannel _channel)
{
	flush_locks[_channel].Acquire();
	//merge_locks[_channel].Wait();

	//history_locks[_channel].Acquire();
	//message_history[_channel].emplace(LogMessage{ _channel, LogLevel::kLogDebug, "Thread " + std::to_string(globals::thread_index) + " started flushing" });
	//history_locks[_channel].Release();

	MergeAll(_channel);

	history_locks[_channel].Acquire();
	std::multiset<LogMessage>	history_copy = message_history[_channel];
	message_history[_channel].clear();
	history_locks[_channel].Release();

	int pair_index = 0;
	for (auto &&pair : history_copy)
	{
		file_streams[_channel] << pair_index++ << " ";
		StreamMessage(file_streams[_channel], pair);
	}

	//history_locks[_channel].Acquire();
	//message_history[_channel].emplace(LogMessage{ _channel, LogLevel::kLogDebug, "Thread " + std::to_string(globals::thread_index) + " flushed to file" });
	//history_locks[_channel].Release();

	flush_locks[_channel].Release();
}

void
CentralLogger::MergeAll(LogChannel _channel)
{
	for (uint32_t i = 0; i < (uint32_t)thread_count; ++i)
	{
		local_loggers[i].channel_locks[_channel].Acquire();
		std::vector<LogMessage>	&messages = local_loggers[i].message_buffers[_channel];
		std::vector<LogMessage>	messages_copy = messages;
		messages.erase(messages.begin(), messages.end());
		local_loggers[i].channel_locks[_channel].Release();

		history_locks[_channel].Acquire();
		for (auto &&message : messages_copy)
			message_history[_channel].emplace(std::move(message));
		history_locks[_channel].Release();
	}
}
#elif defined VERSION_1
void
CentralLogger::Merge(LogChannel _channel, std::vector<LogMessage> &_messages)
{
	merge_locks[_channel].Acquire();

	//message_history[_channel].emplace(LogMessage{ _channel, LogLevel::kLogDebug, "Thread " + std::to_string(globals::thread_index) + " started merging" });

	for (auto &&message : _messages)
		message_history[_channel].emplace(std::move(message));
	_messages.erase(_messages.begin(), _messages.end());
	// We could signal local loggers that are waiting on the flush that they can get back to logging.

	if (message_history[_channel].size() >= buffer_max_size && !flush_locks[_channel].is_locked())
		Flush(_channel, true);
	else
	{
		//message_history[_channel].emplace(LogMessage{ _channel, LogLevel::kLogDebug, "Thread " + std::to_string(globals::thread_index) + " merged to central" });
		merge_locks[_channel].Release();
	}
}

void
CentralLogger::Flush(LogChannel _channel, bool _release_merge_lock)
{
	flush_locks[_channel].Acquire();

	//message_history[_channel].emplace(LogMessage{ _channel, LogLevel::kLogDebug, "Thread " + std::to_string(globals::thread_index) + " started flushing" });

	for (int i = 0; i < thread_count; ++i)
	{
		local_loggers[i].buffer_locks[_channel].Acquire();

		std::vector<LogMessage> &messages = local_loggers[i].message_buffers[_channel];
		for (auto &&message : messages)
			message_history[_channel].emplace(std::move(message));
		messages.erase(messages.begin(), messages.end());
	
		local_loggers[i].buffer_locks[_channel].Release();
	}

	std::multiset<LogMessage>	history_copy = message_history[_channel];
	message_history[_channel].clear();
	flush_locks[_channel].Release();

	write_locks[_channel].Acquire();
	if (_release_merge_lock)
		merge_locks[_channel].Release();

	int pair_index = 0;
	for (auto &&pair : history_copy)
	{
		file_streams[_channel] << pair_index++ << " ";
		StreamMessage(file_streams[_channel], pair);
	}

	//message_history[_channel].emplace(LogMessage{ _channel, LogLevel::kLogDebug, "Thread " + std::to_string(globals::thread_index) + " flushed to file" });

	write_locks[_channel].Release();
}
#else
void
CentralLogger::Merge(LogChannel _channel, std::vector<LogMessage> &_messages)
{
	message_history[_channel].emplace(LogMessage{ _channel, LogLevel::kLogDebug, "Thread " + std::to_string(globals::thread_index) + " started merging" });
	
	merge_locks[_channel].Acquire();
	for (auto &&message : _messages)
		message_history[_channel].emplace(message);
	_messages.erase(_messages.begin(), _messages.end());

	size_t history_size = message_history[_channel].size();
	merge_locks[_channel].Release();

	if (history_size >= buffer_max_size)
		Flush(_channel);

	message_history[_channel].emplace(LogMessage{ _channel, LogLevel::kLogDebug, "Thread " + std::to_string(globals::thread_index) + " merged to central" });

}

void
CentralLogger::Flush(LogChannel _channel)
{
	flush_locks[_channel].Acquire();

	flush_barriers[_channel].Acquire();
	message_history[_channel].emplace(LogMessage{ _channel, LogLevel::kLogDebug, "Thread " + std::to_string(globals::thread_index) + " started flushing" });

	merge_locks[_channel].Acquire();
	for (uint32_t i = 0; i < (uint32_t)thread_count; ++i)
	{
		local_loggers[i].log_locks[_channel].Wait();

		{
			std::vector<LogMessage>	&messages = local_loggers[i].message_buffers[_channel];

			for (auto &&message : messages)
				message_history[_channel].emplace(message);
			messages.erase(messages.begin(), messages.end());
		}
		{
			std::vector<LogMessage>	&messages = local_loggers[i].message_merge_buffer[_channel];

			for (auto &&message : messages)
				message_history[_channel].emplace(message);
			messages.erase(messages.begin(), messages.end());
		}
	}
	merge_locks[_channel].Release();

	std::multiset<LogMessage> history_copy = message_history[_channel];
	message_history[_channel].clear();

	flush_barriers[_channel].Release();

	int pair_index = 0;
	for (auto &&pair : history_copy)
	{
		file_streams[_channel] << pair_index++ << " ";
		StreamMessage(file_streams[_channel], pair);
	}

	message_history[_channel].emplace(LogMessage{ _channel, LogLevel::kLogDebug, "Thread " + std::to_string(globals::thread_index) + " flushed to file" });

	flush_locks[_channel].Release();
}
#endif

LocalLogger&
CentralLogger::operator[](uint32_t _index)
{
	// ASSERT _index validity
	return local_loggers[_index];
}

void
CentralLogger::StreamMessage(std::ostream &_stream, LogMessage const &_message)
{
	uint32_t	info_count = 0;
	std::string level;
	switch (_message.level)
	{
	case kLogDebug:
		level = "  DEBUG";
		break;
	case kLogInfo:
		info_message_count_lock.Acquire();
		info_count = ++info_message_count[_message.channel];
		info_message_count_lock.Release();
		level = "   INFO";
		break;
	case kLogError:
		level = "  ERROR";
		break;
	case kLogWarning:
		level = "WARNING";
		break;
	}
	_stream << std::to_string(info_count) << " " << level << " : " << std::to_string(_message.date.count()) << ", " << _message.message << std::endl;
}