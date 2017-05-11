
#include <cstdint>
#include <thread>
#include "logger.hpp"
#include "logger.h"

namespace globals {

static CentralLogger			central_logger{};
static tools::Logger<256, 16>	main_logger{};

} // namespace globals

void LogTests(uint32_t _thread_index)
{
	globals::thread_index = _thread_index;
	for (uint32_t i = 0; i < 513; ++i)
	{
		std::string message = "Thread " + std::to_string(_thread_index) + " iter " + std::to_string(i);
		//globals::central_logger[_thread_index].Log(kLogGeneral, kLogInfo, message);
		globals::central_logger[_thread_index].Log(kLogGeometry, kLogInfo, message);
		globals::central_logger[_thread_index].Log(kLogMemory, kLogInfo, message);
	}
}
/*
void LogIntegrationTests(uint32_t _thread_index)
{
	globals::main_logger.thread_index = _thread_index;
	for (uint32_t i = 0; i < 513; ++i)
	{
		std::string message = "Thread " + std::to_string(_thread_index) + " iter " + std::to_string(i);
		globals::main_logger.Log(tools::kChannelGeneral, tools::kLevelInfo, message);
	}
}
*/

int main()
{
	const size_t thread_count = 4;
	
#if 0
	globals::main_logger.BindPath(tools::kChannelGeneral, "general.log");
	{
		globals::main_logger.AllowMultipleThreads(thread_count);

		std::thread threads[thread_count];
		std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
		for (uint32_t i = 0; i < thread_count; ++i)
			threads[i] = std::thread{ LogIntegrationTests, i };
		for (uint32_t i = 0; i < thread_count; ++i)
			threads[i].join();
		std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();

		globals::main_logger.Log(tools::kChannelGeneral, tools::kLevelDebug, "total execution : " + std::to_string((end - start).count()));
	}
#endif
	{
		globals::central_logger.SetThreadCount(thread_count);

		std::thread threads[thread_count];

		std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
		for (uint32_t i = 0; i < thread_count; ++i)
			threads[i] = std::thread{ LogTests, i };

		for (uint32_t i = 0; i < thread_count; ++i)
			threads[i].join();
		std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();

		globals::central_logger[0].Log(kLogGeneral, kLogDebug, "total execution : " + std::to_string((end - start).count()));
	}

	return 0;
}