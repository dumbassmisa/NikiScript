#pragma once

#include "Context.h"
#include "Token.h"

#ifndef NS_LOOP_VARIABLE
#define NS_LOOP_VARIABLE '!'
#endif

#ifndef NS_TOGGLE_ON
#define NS_TOGGLE_ON '+'
#endif

#ifndef NS_TOGGLE_OFF
#define NS_TOGGLE_OFF '-'
#endif

#ifndef NS_FILE_EXTENSION
#define NS_FILE_EXTENSION ".cfg"
#endif

#include "API.h"

namespace ns {
	/**
	 * @brief Clear anything related to the current lexer statement
	 * @note It does not go to the next EOS
	 */
	NS_API void clearStatementData(CommandContext* pCtx);

	/**
	 * @brief if variable is a toggle variable and its allowed to run or if it's a common variable.
	 * It's also mandatory to call this function so that it updates those special variables
	 *
	 * @note loop variables are only appended/erased from Context::LoopVariablesRunning
	 * @param pCtx
	 * @see ns::parse
	 * @see ns::handleIdentifierToken
	 */
	NS_API bool canRunVariable(CommandContext* pCtx);

	/**
	 * @brief Called in parse function when EOS or END is reached
	 * @note Sets pCtx->pCommand to nullptr
	 * @note pCtx->pCommand should point to a valid Command object
	 * @param pCtx
	 * @param pProgramVar if not null: sets/gets program variable
	 * @see ns::parse
	 */
	NS_API void handleCommandCall(CommandContext* pCtx);

	/**
	 * @brief Called in parse function when IDENTIFIER token is passed
	 * 
	 * @param pCtx
	 * @param pProgramVar
	 * @return 0 = it called advanceUntil(EOS) 1 = should call lexer.advance() 2 = if it's a variable and should run it
	 * @see ns::parse
	 * @see ns::canRunVariable
	 */
	NS_API TokenFlag handleIdentifierToken(CommandContext* pCtx, bool printError);

	/**
	 * @brief Handles references in the string and checks if the parameter matches the argument type
	 * @note Possible argument types: s = string, i = integer, d = decimal, v = variable
	 * @see ns::parse
	 */
	NS_API void handleArgumentToken(CommandContext* pCtx, bool printError);

	/**
	 * @brief Called in parse function when an ConsoleVariable is passed as a IDENTIFIER
	 * @note I find this algorithm very interesting and fun(it creates a bunch of lexers for each variable call instead of recursive calling)
	 * @see ns::Context::maxConsoleVariablesRecursiveDepth
	 * @param pCtx
	 * @param pProgramVar
	 */
	NS_API void handleConsoleVariableCall(CommandContext* pCtx, bool printError);

	/**
	 * @brief if a loop variable is active, its script is ran here
	 * @param pCtx
	 */
	NS_API void updateLoopVariables(Context* pCtx);

	/**
	 * @brief Parses and interpret scripts: handles commands and variables as well as their arguments
	 * @param pCtx
	 */
	NS_API void parse(CommandContext* pCtx, bool printError=true);
	NS_API void parseUntilEOS(CommandContext* pCtx, bool printError=true);
	NS_API bool parseFile(CommandContext* pCtx, const char* path, bool printError=true);
}