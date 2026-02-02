#pragma once

#include <vector>
#include <stdint.h>
#include <string>
#include <unordered_map>

#include "Command.h"
#include "CommandHandler.h"
#include "Print.h"
#include "ProgramVariable.h"
#include "Lexer.h"

#include "CS.h"
#include "DLL.h"

namespace ns {
	using Origin = uint16_t;

	enum OriginType : Origin {
		COMMAND = 1, ///< If a command is calling another command.
		VARIABLE = 2, ///< Any variable.
		VARIABLE_IN_VARIABLE = 4, ///< Var x that calls var y.
		VARIABLE_LOOP = 8, ///< '!'.
		VARIABLE_TOGGLE = 16, ///< '+' or '-'.
		FILE = 32, ///< ns::parseFile or exec command.
		INTERNAL = 64, ///< Raw script generated from C++ code and not from a file or variable or anything else.
		REFERENCE = 128, ///< Scripts ran inside a reference ${echo hello} -> echo command is ran inside a reference and its output is the reference result.
	};

	struct NS_API Arguments {
		std::vector<std::string> arguments{};
#ifdef NS_CS_API
		size_t count() const;
#endif

		std::string& getString(size_t index);

		float getFloat(size_t index);
		double getDouble(size_t index);
		long double getLongDouble(size_t index);

		/**
		 * @brief Uses std::stoul so any number below that can be used.
		 * @tparam T Number type.
		 */
		template<typename T>
		T getUnsigned(size_t index) {
			return static_cast<T>(std::stoull(arguments[index]));
		}

#ifdef NS_CS_API
		uint8_t getByte(size_t index);
		uint16_t getUInt16(size_t index);
		uint32_t getUInt32(size_t index);
		uint64_t getUInt64(size_t index);
#endif

		/**
		 * @brief Uses std::stoi so any number below that can be used.
		 * @tparam T Return type.
		 */
		template<typename T>
		T getSigned(size_t index) {
			return static_cast<T>(std::stoi(arguments[index]));
		}

#ifdef NS_CS_API
		int8_t getSByte(size_t index);
		int16_t getInt16(size_t index);
		int32_t getInt32(size_t index);
		int64_t getInt64(size_t index);
#endif
	};

	using ConsoleVariables = std::unordered_map<std::string, std::string>;
	using LoopVariablesRunning = std::vector<ConsoleVariables::pointer>;
	using ToggleVariablesRunning = std::vector<ConsoleVariables::pointer>; ///< This is unecessary to be a pointer but I like the idea of using only 8 bytes instead of the same bytes as the var name.
	using ToggleCommandsRunning = std::vector<Command*>;

	/**
	 * @brief General context that can be used with multiple command contexts.
	 */
	struct NS_API Context {
		ConsoleVariables consoleVariables{};
		ProgramVariables programVariables{};

		CommandHandler commands{};

		LoopVariablesRunning loopVariablesRunning{};
		ToggleVariablesRunning toggleVariablesRunning{};
		ToggleCommandsRunning toggleCommandsRunning{};

		uint16_t maxConsoleVariablesRecursiveDepth = 255; ///< How many console variables can be called inside each other. 0 = unlimited
		const char* cfgDirectory = nullptr; ///< Expects a null terminated char array. Heap allocated is possible but this code doesn't free by itself

#ifdef NS_CS_API
		NS_CS_UNORDERED_MAP(ConsoleVariables, const std::string&, std::string&);
		NS_CS_UNORDERED_MAP(ProgramVariables, const std::string&, ns::ProgramVariable&);
		NS_CS_VECTOR_NO_PUSH(LoopVariablesRunning, std::string&, const std::string&);
		NS_CS_VECTOR_NO_PUSH(ToggleVariablesRunning, std::string&, const std::string&);
		NS_CS_VECTOR(ToggleCommandsRunning, ns::Command*, ns::Command*);
#endif
	};

	/**
	 * @brief Context passed to commands callbacks which contains command required data.
	 */
	struct NS_API CommandContext {
		CommandContext();
		CommandContext(Context* pCtx);
		CommandContext(Context* pCtx, const std::string& lexerInput);
		CommandContext(Context* pCtx, const char* lexerInput);

		Context* pCtx = nullptr; ///< A general context which contains data which commands won't need unless for special cases.
		Command* pCommand = nullptr;
		Lexer lexer{};

		Arguments args{};

		std::string filePath{}; ///< when running script from a file.
		size_t lineCount = 0;

		Origin origin = 0; ///< this is used so that the command knows where he's running in. See ns::OriginType.
	};

	/**
	 * @brief If you want a 100% new copy without being dependent on the source context then use this function.
	 * LoopVariablesRunning and ToggleVariablesRunning stores pointers
	 * to ConsoleVariables as well as toggleCommandsRunning whose
	 * pointers are from CommandHandler. That's why this function exists:
	 * It updates all those pointers.
	 * @param source Object to copy content from.
	 */
	NS_API Context deepCopyContext(const Context* source);
}

NS_API ns::Origin operator|(ns::OriginType l, ns::OriginType r);
NS_API ns::Origin operator|(ns::Origin l, ns::OriginType r);
NS_API ns::Origin operator|(ns::OriginType l, ns::Origin r);
NS_API ns::Origin& operator|=(ns::Origin& l, ns::OriginType r);
NS_API ns::Origin operator&(ns::Origin l, ns::OriginType r);
NS_API ns::Origin operator&(ns::OriginType l, ns::Origin r);
NS_API ns::Origin& operator&=(ns::Origin& l, ns::OriginType r);
NS_API ns::Origin operator~(ns::OriginType l);