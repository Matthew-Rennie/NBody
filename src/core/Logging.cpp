#include "stdafx.hpp"
#include "Logging.h"
#include <iostream>

#if CB_LOGGING_USE_FILE_NAME
void core::LogInternal::Log(const char* file, const char* func, int line, const char* message)
{
	std::string _file = file;
	std::string _func = func;
	std::string _line = std::to_string(line);
	std::string _message = message;
	std::string result = "[" + _file + "] [" + _func + " #" + _line + "] " + _message + "\n";
#if CB_LOGGING_OUTPUT_CONSOLE
#if CB_IMMEDIATE_FLUSH_STREAM
	std::cerr << result;
#else
	std::clog << result;
#endif
#endif
#if CB_LOGGING_OUTPUT_FILE 
	log_message_queue.push(result);
#endif
}
#else
void core::LogInternal::Log(const char* func, int line, const char* message)
{
	std::string _func = func;
	std::string _line = std::to_string(line);
	std::string _message = message;
	std::string result = "[" + _func + " #" + _line + "] " + _message + "\n";

#if CB_LOGGING_OUTPUT_CONSOLE
#if CB_IMMEDIATE_FLUSH_STREAM
	std::cerr << result;
#else
	std::clog << result;
#endif
#endif
#if CB_LOGGING_OUTPUT_FILE 
	log_message_queue.push(result);
#endif
}
#endif

void core::LogInternal::WriteQueueToDisk()
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

void core::LogInternal::InitDisk()
{
#if CB_LOGGING_OUTPUT_FILE 
	file.open(CB_LOGGING_FILENAME);
	file.clear();
#endif

}

void core::LogInternal::ReleaseDisk()
{
#if CB_LOGGING_OUTPUT_FILE 
	file.close();
#endif
}

