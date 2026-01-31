#pragma once

#include <string>

#include "Context.h"

#include "DLL.h"

namespace ns {
	NS_API void help(CommandContext* pCtx, const std::string& name);

	/**
	 * @brief shows command usage
	 * @param pCtx
	 * @note s[command?]
	 */
	NS_API void help_command(CommandContext* pCtx, void*);

	/**
	 * @brief prints all the arguments passed
	 * @param pCtx
	 * @note s[message]
	 */
	NS_API void echo_command(CommandContext* pCtx, void*);

	/**
	 * @brief creates a variable
	 * @param pCtx
	 * @param name
	 * @param value
	 * @return true on success
	 * @note if origin contains OriginType::REFERENCE then it will not print errors
	 */
	NS_API bool var(CommandContext* pCtx, const std::string& name, const std::string& value);

	/**
	 * @brief creates a variable
	 * @param pCtx
	 * @note s[name] s[value]
	 * @see ns::var
	 */
	NS_API void var_command(CommandContext* pCtx, void*);

	NS_API void delvar(CommandContext* pCtx, const std::string& name);

	/**
	 * @brief deletes a variable
	 * @param pCtx
	 * @note v[consoleVariable]
	 * @see ns::delvar
	 */
	NS_API void delvar_command(CommandContext* pCtx, void*);

	NS_API void incrementvar(CommandContext* pCtx, const std::string& name, float min, float max, float delta);

	/**
	 * @brief increments a variable value
	 * @param pCtx
	 * @note v[variable] d[min] d[max] d[delta?]
	 * @see ns::incrementvar
	 */
	NS_API void incrementvar_command(CommandContext* pCtx, void*);

	/**
	 * @brief toggles a variable value between option1 and option2
	 * @param pCtx
	 * @param variable
	 * @param varName
	 * @param option1
	 * @param option2
	 */
	NS_API void toggle(CommandContext* pCtx, const std::string& varName, const std::string& option1, const std::string& option2);

	/**
	 * @brief toggles a variable value between option1 and option2
	 * @param pCtx
	 * @note v[variable] s[option1] s[option2]
	 * @see ns::toggle
	 */
	NS_API void toggle_command(CommandContext* pCtx, void*);

	/**
	 * @brief parses a file as nikiscript
	 * @param pCtx
	 * @note s[filePath]
	 * @see ns::parseFile
	 */
	NS_API void exec_command(CommandContext* pCtx, void*);

	/**
	 * @brief adds default commands such as echo
	 * @param pCtx
	 * @see ns::echo_command
	 */
	NS_API void registerCommands(Context* pCtx);

	/**
	 * @brief creates a variable and stores it in ns::Context::programVariables
	 * @param pCtx
	 * @param name
	 * @param description
	 * @param pVar
	 * @param get
	 * @param set
	 */
	NS_API void registerVariable(Context* pCtx, const std::string& name, const std::string& description, void* pVar, const GetProgramVariableValueFn& get, const SetProgramVariableValueFn& set);
}