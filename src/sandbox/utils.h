#pragma once

#include <iostream>
#include <string>
#include <string_view>
#include <sstream>
#include <stdexcept>

namespace utils
{
	// https://codereview.stackexchange.com/q/269425

	void format_helper(std::ostringstream& oss, std::string_view str) // base function
	{
		oss << str;
	}

	template<typename T, typename... Targs>
	void format_helper(std::ostringstream& oss, std::string_view str, T value, Targs... args)
	{
		std::size_t openBracket = str.find_first_of('{');
		if (openBracket != std::string::npos)
		{
			std::size_t closeBracket = str.find_first_of('}', openBracket + 1);
			if (closeBracket == std::string::npos)
				throw std::runtime_error("missing closing bracket.");
			oss << str.substr(0, openBracket);
			oss << value;
			format_helper(oss, str.substr(closeBracket + 1), args...);
			return;
		}
		oss << str;
	}

	std::string format(std::string_view str)
	{
		return std::string(str);
	}

	template<typename T, typename... Targs>
	std::string format(std::string_view str, T value, Targs...args)
	{
		std::ostringstream oss;
		format_helper(oss, str, value, args...);
		return oss.str();
	}
}