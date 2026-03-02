#pragma once

#include <stdint.h>
#include <string>
#include <sstream>

#include "API.h"

namespace ns {
	/**
	 * @warning this function is not meant to be used outside this header
	 * @tparam  
	 * @param format
	 * @param buf
	 */
	template<typename ...>
	static void _formatStringValue(const std::string& format, std::stringstream& buf) {
		buf << format;
	}

	/**
	 * @warning this function is not meant to be used outside this header
	 * @tparam T
	 * @tparam Args
	 * @param format
	 * @param buf
	 * @param value
	 * @param args
	 */
	template<typename T, typename ... Args>
	static void _formatStringValue(const std::string& format, std::stringstream& buf, T value, Args& ... args) {
		size_t idx = format.find("{}");
		if (idx == std::string::npos) {
			buf << format;
			return;
		}

		buf << format.substr(0, idx) << value;
		
		_formatStringValue(format.substr(idx+2), buf, args...);
	}

	/**
	 * @brief format string using {}. Example: `formatString("Hello, {}!", "World")` -> `"Hello, World!"`
	 * @tparam Args
	 * @param format
	 * @param args
	 * @return std::string
	 */
	template<typename ... Args>
	std::string formatString(const std::string& format, Args ... args) {
		std::stringstream buf{};
		_formatStringValue(format, buf, args...);
		return buf.str();
	}

	/**
	 * @brief call isspace but does not accept newline
	 * @param c char to check if is space
	 * @return true if is space
	 * @return false if is not space
	 */
	NS_API bool isSpaceNotNewline(unsigned char c);
}