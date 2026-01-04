#pragma once

#include <vector>
#include <string>
#include <stdint.h>

#include "DLL.h"

namespace ns {
	struct CommandContext;

	using CommandCallback = void(*)(CommandContext* pCtx, void* pData);

	struct NS_API Command {
		std::string name{};
		std::string description{};

		/**
		 * @note odd = name
		 * @note even = description
		 */
		std::vector<std::string> argsDescriptions{};

		uint8_t minArgs = 0, maxArgs = 0;
		CommandCallback callback = nullptr;

		void* pData = nullptr; ///< pData is useful for functions that don't have a defined body and actually depends on a data that it's not possible to know without a hacky way which is worse than just using pData

		Command();
		/**
		 * @param name
		 * @param minArgs
		 * @param maxArgs
		 * @param callback
		 * @param pData for when a command callback depends on different datas and requires the creation a bunch of comands to split its needs
		 * @param description Command description
		 * @param argsDescriptions Arguments description. Should have 2 strings for each argument, where the first one is argument name and the second is argument description.
		 * @see ns::registerCommands for code example
		 */
		Command(const std::string& name, uint8_t minArgs, uint8_t maxArgs, CommandCallback callback, void* pData, const std::string& description, const std::vector<std::string>& argsDescriptions);
	
		/**
		 * @brief splitted by space
		 */
		std::string getArgumentsNames();

		/**
		 * @brief prints usage, description and argsDescriptions all like a data tree
		 */
		void printAsDataTree();
	};
}