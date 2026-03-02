#pragma once

#include <stdint.h>
#include <string>

#include "Token.h"

#ifndef NS_ESCAPE_NEXT_CHAR
#define NS_ESCAPE_NEXT_CHAR '\\'
#endif

#ifndef NS_STATEMENT_SEPARATOR
#define NS_STATEMENT_SEPARATOR ';'
#endif

#ifndef NS_REFERENCE
#define NS_REFERENCE '$'
#endif

#ifndef NS_REFERENCE_OPEN
#define NS_REFERENCE_OPEN '{'
#endif

#ifndef NS_REFERENCE_CLOSE
#define NS_REFERENCE_CLOSE '}'
#endif

#ifndef NS_ARGUMENTS_SEPARATOR
#define NS_ARGUMENTS_SEPARATOR ','
#endif

#ifndef NS_ARGUMENTS_OPEN
#define NS_ARGUMENTS_OPEN '('
#endif

#ifndef NS_ARGUMENTS_CLOSE
#define NS_ARGUMENTS_CLOSE ')'
#endif

#ifndef NS_COMMENT_LINE
#define NS_COMMENT_LINE '/'
#endif

#ifndef NS_COMMENT_LINES // Joined together with NS_COMMENT_LINE -> /* This is a comment */
#define NS_COMMENT_LINES '*'
#endif

#ifndef NS_ARGUMENTS_QUOTE
#define NS_ARGUMENTS_QUOTE '"'
#endif

#include "API.h"

namespace ns {
	struct NS_API Lexer {
		std::string input;
		size_t position = 0;
		size_t openArguments = 0; ///< how many times NS_ARGUMENTS_OPEN was found
		size_t lineIndex = 0; ///< how many newlines were found basically

		/**
		 * @brief This variable is where advance stores the token
		 * @see Lexer::advance
		 */
		Token token = {TokenType::NONE};

		Lexer();
		Lexer(const std::string& input);

		Lexer& operator=(const std::string& input);
		Lexer& operator=(const char* input);

		/**
		 * @brief Gets the next token in the input
		 * @note Position is set to the **next** token position in the end of this function
		 * @see Lexer::token
		 * @see Lexer::setTokenValue
		 * @see Lexer::setTokenType
		 */
		void advance();

		/**
		 * @brief Advances tokens until it reaches one of the flags
		 * @param flags Bitwise TokenType
		 * @see Lexer::advance
		 */
		void advanceUntil(uint8_t flags);

		/**
		 * @brief Gets token value by checking where a whitespace is found and updates token position to the end+1 of the new token
		 *
		 * @return true if it NS_STATEMENT_SEPARATOR but inside a string and therefore should not be interpreted as such
		 * @see Lexer::setTokenType
		 */
		bool setTokenValue();

		/**
		 * @brief Identifies token type by checking the previous token type
		 * @see Lexer::setTokenValue
		 */
		void setTokenType(bool skipStatementSeparator);

		/**
		 * @brief resets members
		 */
		void clear();
	};
}