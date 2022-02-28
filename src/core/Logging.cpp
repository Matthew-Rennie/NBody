#include "stdafx.hpp"
#include "Logging.h"
#include <iostream>

#if CB_LOGGING_USE_FILE_NAME
void core::Impl::Log(const char* file, const char* func, int line, const char* message)
{
	std::string _file = file;
	std::string _func = func;
	std::string _line = std::to_string(line);
	std::string _message = message;
	std::string result = "[" + _file + "] [" + _func + " #" + _line + "] " + _message + "\n";
	LogInternal(result.c_str());
}
#else
void core::Impl::Log(const char* func, int line, const char* message)
{
	std::string _func = func;
	std::string _line = std::to_string(line);
	std::string _message = message;
	std::string result = "[" + _func + " #" + _line + "] " + _message + "\n";

	LogInternal(result.c_str());
}
#endif

void core::Impl::WriteQueueToDisk()
{
#if CB_LOGGING_OUTPUT_FILE 
	while (!log_message_queue.empty())
	{
		file << log_message_queue.front();
		log_message_queue.pop();
	}
	file.flush();
#endif
}

void core::Impl::InitDisk()
{
#if CB_LOGGING_OUTPUT_FILE 
	file.open(CB_LOGGING_FILENAME);
	file.clear();
#endif

}

void core::Impl::ReleaseDisk()
{
#if CB_LOGGING_OUTPUT_FILE 
	file.close();
#endif
}

void core::Impl::LogVersion()
{

#if defined _DEBUG && defined ENVIRONMENT32
	LogInternal("Running 32bit Debug mode\n");
#endif

#if defined _DEBUG && defined ENVIRONMENT64
	LogInternal("Running 64bit Debug mode\n");
#endif

#if !defined _DEBUG && defined ENVIRONMENT32
	LogInternal("Running 32bit Release mode\n");
#endif

#if !defined _DEBUG && defined ENVIRONMENT64
	LogInternal("Running 64bit Release mode\n");
#endif

}

void core::Impl::LogInternal(const char* text)
{
#if CB_LOGGING_OUTPUT_CONSOLE
#if CB_IMMEDIATE_FLUSH_STREAM
	std::cerr << text;
#else
	std::clog << result;
#endif
#endif
#if CB_LOGGING_OUTPUT_FILE 
	log_message_queue.push(text);
#endif
}

