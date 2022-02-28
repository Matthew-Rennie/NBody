#pragma once
#define CB_LOGGING_USE_FILE_NAME false
#define CB_LOGGING_OUTPUT_CONSOLE true
#define CB_LOGGING_OUTPUT_FILE true

#define CB_LOGGING_FILENAME "log.txt"

// will replace std::clog with std::cerr to disable console stream buffering
#define CB_IMMEDIATE_FLUSH_STREAM true

#include <string>
#include <queue>
#include <fstream>

#if CB_LOGGING_USE_FILE_NAME

#define CB_LOG(message) core::LogInternal::Log(__FILE__, __FUNCTION__, __LINE__, message)
namespace core::LogInternal
{
	void Log(const char* file, const char* func, int line, const char* message);
}

#else

#define CB_LOG(message) core::LogInternal::Log(__FUNCTION__, __LINE__, message)
namespace core::LogInternal
{
	void Log(const char* func, int line, const char* message);
}

#endif

#define CB_LOG_FRAME core::LogInternal::WriteQueueToDisk()
#define CB_LOG_INIT core::LogInternal::InitDisk()
#define CB_LOG_RELEASE core::LogInternal::ReleaseDisk()

namespace core::LogInternal
{

	static std::queue<std::string> log_message_queue;
	static std::ofstream file;

	void WriteQueueToDisk();
	void InitDisk();
	void ReleaseDisk();
}



