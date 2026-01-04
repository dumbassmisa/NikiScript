#pragma once

#include <string>
#include <cstring>
#include <sstream>
#include <unordered_map>

#include "DLL.h"

namespace ns {
	struct ProgramVariable;
	struct Context;
	struct CommandContext;

	using GetProgramVariableValue = std::string(*)(CommandContext* pCtx, ProgramVariable* pVar);
	using SetProgramVariableValue = void(*)(CommandContext* pCtx, ProgramVariable* pVar, const std::string& str);

	struct NS_API ProgramVariable {
		void* pValue = nullptr;
		std::string description;

		GetProgramVariableValue get = nullptr;
		SetProgramVariableValue set = nullptr;

		ProgramVariable();
		ProgramVariable(void* pValue, const std::string& description, const GetProgramVariableValue& get, const SetProgramVariableValue& set);
	};

	using ProgramVariables = std::unordered_map<std::string, ProgramVariable>;

	NS_API std::string getString(CommandContext*, ProgramVariable* pVar);
	NS_API void setString(CommandContext*, ProgramVariable* pVar, const std::string& str);

	NS_API std::string getCharArray(CommandContext*, ProgramVariable* pVar);

	template<size_t Size = 256>
	void setCharArray(CommandContext*, ProgramVariable* pVar, const std::string& str) {
		try {
			char* pValue = static_cast<char*>(pVar->pValue);
			if (str.size() == 0) {
				pValue[0] = '\0';
				return;
			}

			size_t i = 0;
			for (; i < Size-1 && i < str.size(); ++i)
				pValue[i] = str[i];

			pValue[i] = '\0';

			//strncpy_s(pValue, Size, str.c_str(), Size);
			//pValue[Size - 1] = '\0'; // Ensure null termination
		} catch (...) {}
	}

	template<typename T>
	std::string getNumber(CommandContext*, ProgramVariable* pVar) {
		return std::to_string(*static_cast<T*>(pVar->pValue));
	}

	/**
	 * @brief uses std::stoul so any number below that can be used
	 * @tparam T number type
	 * @param pCtx
	 * @param pVar 
	 * @param str
	 */
	template<typename T>
	void setUnsigned(CommandContext*, ProgramVariable* pVar, const std::string& str) {
		try {
			*static_cast<T*>(pVar->pValue) = (T)std::stoul(str);
		} catch (...) {}
	}

	NS_API void setUnsignedLongLong(CommandContext*, ProgramVariable* pVar, const std::string& str);

	NS_API void setFloat(CommandContext*, ProgramVariable* pVar, const std::string& str);
	NS_API void setDouble(CommandContext*, ProgramVariable* pVar, const std::string& str);
	NS_API void setLongDouble(CommandContext*, ProgramVariable* pVar, const std::string& str);

	NS_API void setChar(CommandContext*, ProgramVariable* pVar, const std::string& str);
	NS_API void setShort(CommandContext*, ProgramVariable* pVar, const std::string& str);
	NS_API void setInteger(CommandContext*, ProgramVariable* pVar, const std::string& str);
	NS_API void setLong(CommandContext*, ProgramVariable* pVar, const std::string& str);
	NS_API void setLongLong(CommandContext*, ProgramVariable* pVar, const std::string& str);

	template<typename T, T bit>
	void setBit(CommandContext*, ProgramVariable* pVar, const std::string& str) {
		try {
			if (std::stol(str) > 0) {
				*static_cast<T*>(pVar->pValue) |= bit;
			} else {
				*static_cast<T*>(pVar->pValue) &= ~(bit);
			}
		} catch (...) {}
	}

	template<typename T, T bit>
	std::string getBit(CommandContext*, ProgramVariable* pVar) {
		return (*static_cast<T*>(pVar->pValue) & bit)? std::string("1") : std::string("0");
	}
}