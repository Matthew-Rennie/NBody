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

#define CB_LOG(message) core::Impl::Log(__FILE__, __FUNCTION__, __LINE__, message)
namespace core::Impl
{
	void Log(const char* file, const char* func, int line, const char* message);
	
	inline void Log(const char* file, const char* func, int line, std::string message)
	{
		Log(file, func, line, message.c_str());
	}
	template<typename T>
	inline void Log(const char* file, const char* func, int line, T message)
	{
		Log(file, func, line, std::to_string(message).c_str());
	}
}

#else

#define CB_LOG(message) core::Impl::Log(__FUNCTION__, __LINE__, message)
namespace core::Impl
{
	void Log(const char* func, int line, const char* message);

	inline void Log(const char* func, int line, std::string message)
	{
		Log(func, line, message.c_str());
	}
	template<typename T>
	inline void Log(const char* func, int line, T message)
	{
		Log(func, line, std::to_string(message).c_str());
	}
}

#endif

#define CB_LOG_FRAME core::Impl::WriteQueueToDisk()
#define CB_LOG_INIT core::Impl::InitDisk()
#define CB_LOG_RELEASE core::Impl::ReleaseDisk()
#define CB_LOG_VERSION core::Impl::LogVersion()

namespace core::Impl
{

	static std::queue<std::string> log_message_queue;
	static std::ofstream file;

	void WriteQueueToDisk();
	void InitDisk();
	void ReleaseDisk();
	void LogVersion();
	void LogInternal(const char* text);
}



