#pragma once

#include <unordered_map>
#include <string>

#include "Command.h"

#include "API.h"

#define nsRegisterCommand(pCtx, name, minArgs, maxArgs, callback, description, ...) \
	(pCtx)->commands.add(ns::Command(name, minArgs, maxArgs, callback, nullptr, description, {__VA_ARGS__}))

#define nsRegisterCommandWithData(pCtx, name, minArgs, maxArgs, pData, callback, description, ...) \
	(pCtx)->commands.add(ns::Command(name, minArgs, maxArgs, callback, pData, description, {__VA_ARGS__}))

namespace ns {
	struct CommandContext;

	struct NS_API CommandHandler {
		std::unordered_map<std::string, Command> commands{};

		Command* get(const std::string& name);
		/**
		 * @brief adds command to commands unordered_map
		 * 
		 * @param command name of the will be mapped in the commands variable
		 * @return true if command name is okay
		 * @return false if command with this name already exists
		 */
		bool add(const Command& command);

		void remove(CommandContext* pCtx, const std::string& name);
	};
}