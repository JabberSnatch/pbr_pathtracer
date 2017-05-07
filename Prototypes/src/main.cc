
#include <cstdint>
#include <thread>
#include "logger.hpp"

namespace globals {

static CentralLogger		central_logger{};

} // namespace globals

void LogTests(uint32_t _thread_index)
{
	for (uint32_t i = 0; i < 500; ++i)
	{
		std::string message = "Thread " + std::to_string(_thread_index) + " iter " + std::to_string(i);
		globals::central_logger[_thread_index].Log(kLogGeneral, kLogInfo, message);
		globals::central_logger[_thread_index].Log(kLogGeometry, kLogInfo, message);
		globals::central_logger[_thread_index].Log(kLogMemory, kLogInfo, message);
	}
}

int main()
{
	const size_t thread_count = 4;
	globals::central_logger.SetThreadCount(thread_count);

	std::thread threads[thread_count];
	for (uint32_t i = 0; i < thread_count; ++i)
		threads[i] = std::thread{ LogTests, i };

	for (uint32_t i = 0; i < thread_count; ++i)
		threads[i].join();

	return 0;
}