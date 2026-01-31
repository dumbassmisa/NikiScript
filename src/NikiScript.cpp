#include "NikiScript.h"

#include <sstream>

#include "Utils.h"
#include "Parser.h"
#include "Lexer.h"

void ns::help(CommandContext* pCtx, const std::string& name) {
	if (name.empty()) {
		std::stringstream oss{};
		for (auto& command : pCtx->pCtx->commands.commands)
			oss << command.second.name << ' ' << command.second.getArgumentsNames() << '\n';

		ns::print(ns::ECHO, oss.str().c_str());

	} else {
		Command* pCommand = pCtx->pCtx->commands.get(name);
		if (pCommand == nullptr) {
			ns::printf(ns::ERROR, "Command \"{}\" not found\n", name);
			return;
		}

		pCommand->printAsDataTree();
	}
}

void ns::help_command(CommandContext* pCtx, void*) {
	help(pCtx, pCtx->args.arguments.size() > 0? pCtx->args.getString(0) : "");
}

void ns::echo_command(CommandContext* pCtx, void*) {
	ns::printf(ns::ECHO, "{}\n", pCtx->args.getString(0));
}

bool ns::var(CommandContext* pCtx, const std::string& name, const std::string& value) {
	if (name.empty()) {
		ns::print(PrintLevel::ERROR, "Variable name can not be empty\n");
		return false;
	}

	if (isdigit(static_cast<unsigned char>(name[0]))) {
		ns::print(PrintLevel::ERROR, "Variable name can not contain digit in the beginning\n");
		return  false;
	}

	for (size_t i = 0; i < name.size(); ++i) {
		if (isspace(static_cast<unsigned char>(name[i]))) {
			ns::print(PrintLevel::ERROR, "Variable name can not contain whitespace\n");
			return  false;
		}

		switch (name[i]) {
		case NS_LOOP_VARIABLE:
			if (i == 0 && name.size() > 1)
				break;
			ns::printf(PrintLevel::ERROR, "Can not use {} in a variable name alone or after the first character\n", NS_LOOP_VARIABLE);
			return false;

		case NS_TOGGLE_ON:
			if (i == 0 && name.size() > 1)
				break;
			ns::printf(PrintLevel::ERROR, "Can not use {} in a variable name alone or after the first character\n", NS_TOGGLE_ON);
			return false;

		case NS_TOGGLE_OFF:
			if (i == 0 && name.size() > 1)
				break;
			ns::printf(PrintLevel::ERROR, "Can not use {} in a variable name alone or after the first character\n", NS_TOGGLE_OFF);
			return false;

		case NS_REFERENCE:
			ns::printf(PrintLevel::ERROR, "Can not use {} in a variable name\n", NS_REFERENCE);
			return false;

		case NS_REFERENCE_OPEN:
			ns::printf(PrintLevel::ERROR, "Can not use {} in a variable name\n", NS_REFERENCE_OPEN);
			return false;

		case NS_REFERENCE_CLOSE:
			ns::printf(PrintLevel::ERROR, "Can not use {} in a variable name\n", NS_REFERENCE_CLOSE);
			return false;

		case NS_ARGUMENTS_SEPARATOR:
			ns::printf(PrintLevel::ERROR, "Can not use {} in a variable name\n", NS_ARGUMENTS_SEPARATOR);
			return false;

		case NS_ARGUMENTS_CLOSE:
			ns::printf(PrintLevel::ERROR, "Can not use {} in a variable name\n", NS_ARGUMENTS_CLOSE);
			return false;

		case NS_ARGUMENTS_OPEN:
			ns::printf(PrintLevel::ERROR, "Can not use {} in a variable name\n", NS_ARGUMENTS_OPEN);
			return false;

		case NS_STATEMENT_SEPARATOR:
			ns::printf(PrintLevel::ERROR, "Can not use {} in a variable name\n", NS_STATEMENT_SEPARATOR);
			return false;

		case NS_COMMENT_LINE:
			ns::printf(PrintLevel::ERROR, "Can not use {} in a variable name\n", NS_COMMENT_LINE);
			return false;

		case NS_COMMENT_LINES:
			ns::printf(PrintLevel::ERROR, "Can not use {} in a variable name\n", NS_COMMENT_LINES);
			return false;
		}
	}

	if (pCtx->pCtx->programVariables.count(name) != 0) {
		ns::print(PrintLevel::ERROR, "A program variable already uses this name and therefore can not be replaced\n");
		return false;
	}

	if (pCtx->pCtx->commands.commands.count(name) != 0) {
		ns::print(PrintLevel::ERROR, "A command already uses this name and therefore can not be replaced\n");
		return false;
	}

	pCtx->pCtx->consoleVariables[name] = value;
	return true;
}

void ns::var_command(CommandContext* pCtx, void*) {
	bool result = var(pCtx, pCtx->args.getString(0), pCtx->args.arguments.size() > 1? pCtx->args.getString(1) : "");
	if (pCtx->origin & OriginType::REFERENCE)
		ns::printf(ns::ECHO, "{}\n", result);
}

void ns::delvar(CommandContext* pCtx, const std::string& name) {
	if (pCtx->pCtx->consoleVariables.count(name) == 0) {
		ns::printf(PrintLevel::ERROR, "Expected console variable\n");
		return;
	}

	for (size_t i = 0; i < pCtx->pCtx->loopVariablesRunning.size(); ++i) {
		if (pCtx->pCtx->loopVariablesRunning[i]->first == name) {
			pCtx->pCtx->loopVariablesRunning.erase(pCtx->pCtx->loopVariablesRunning.begin()+i);
			break;
		}
	}

	if (name.size() > 1 && name[0] == '+') {
		std::string toggleVarName = name.substr(1);

		for (size_t i = 0; i < pCtx->pCtx->toggleVariablesRunning.size(); ++i) {
			if (pCtx->pCtx->toggleVariablesRunning[i]->first == toggleVarName) {
				pCtx->pCtx->toggleVariablesRunning.erase(pCtx->pCtx->toggleVariablesRunning.begin()+i);
				break;
			}
		}
	}

	pCtx->pCtx->consoleVariables.erase(name);
}

void ns::delvar_command(CommandContext* pCtx, void*) {
	delvar(pCtx, pCtx->args.getString(0));
}

void ns::incrementvar(CommandContext* pCtx, const std::string& name, float min, float max, float delta) {
	if (min > max) {
		ns::printf(ns::ERROR, "max({}) should be higher than min({})\n", max, min);
		return;
	}

	float value = 0.0;
	if (pCtx->pCtx->consoleVariables.count(name) != 0) {
		try {
			value = std::stof(pCtx->pCtx->consoleVariables[name]);
		} catch (...) {
			ns::printf(ns::ERROR, "\"{}\" is not a number\n", pCtx->pCtx->consoleVariables[name]);
			return;
		}
	} else {
		try {
			value = std::stof(pCtx->pCtx->programVariables[name].get(pCtx, &pCtx->pCtx->programVariables[name]));
		} catch (...) {
			ns::printf(ns::ERROR, "\"{}\" is not a number\n", pCtx->pCtx->consoleVariables[name]);
			return;
		}
	}

	value += delta;
	if (value > max)
		value = min;

	if (value < min)
		value = min;

	if (pCtx->pCtx->consoleVariables.count(name) != 0) {
		pCtx->pCtx->consoleVariables[name] = std::to_string(value);
	} else
		pCtx->pCtx->programVariables[name].set(pCtx, &pCtx->pCtx->programVariables[name], std::to_string(value));
}

void ns::incrementvar_command(CommandContext* pCtx, void*) {
	incrementvar(pCtx, pCtx->args.getString(0), pCtx->args.getFloat(1), pCtx->args.getFloat(2), pCtx->args.arguments.size() > 3? pCtx->args.getFloat(3) : 1.0f);
}

void ns::toggle(CommandContext* pCtx, const std::string& varName, const std::string& option1, const std::string& option2) {
	if (pCtx->pCtx->consoleVariables.count(varName) != 0) { // Console Variable
		std::string& varValue = pCtx->pCtx->consoleVariables[varName];

		if (varValue == option1)
			varValue = option2;
		else
			varValue = option1;

	} else if (pCtx->pCtx->programVariables.count(varName) != 0) { // Program Variable
		ns::ProgramVariable& var = pCtx->pCtx->programVariables[varName];
		std::string varValue = var.get(pCtx, &var);

		if (varValue == option1)
			var.set(pCtx, &var, option2);
		else
			var.set(pCtx, &var, option1);

	} else if (ns::Command *pCommand = pCtx->pCtx->commands.get(varName)) { // Command
		void* pOriginalPrintCallbackData = pPrintCallbackData;
		PrintCallbackFn originalPrintCallback = printCallback;

		std::string varValue;
		setPrintCallback(&varValue, printAppendToStringEchoOnly);

		ns::CommandContext ctx;
		ctx.pCtx = pCtx->pCtx;
		ctx.lexer = pCommand->name;
		ns::parse(&ctx);

		setPrintCallback(pOriginalPrintCallbackData, originalPrintCallback);

		if (!varValue.empty() && varValue[varValue.size()-1] == '\n')
			varValue.pop_back();

		if (varValue == option1)
			varValue = option2;
		else
			varValue = option1;

		// This could be unsecure depending on what nikiscript is being used, because if you set one of the options with something like "); destroy_humanity\\\\", it will call destroy_humanity!
		varValue = formatString("{}({})", pCommand->name, varValue.c_str());

		ns::clearStatementData(&ctx);
		ctx.lexer = varValue;
		ns::parse(&ctx);
	} else
		ns::print(PrintLevel::ERROR, "toggle command expected a variable or command\n");
}

void ns::toggle_command(CommandContext* pCtx, void*) {
	ns::toggle(pCtx, pCtx->args.getString(0), pCtx->args.getString(1), pCtx->args.getString(2));
}

void ns::exec_command(CommandContext* pCtx, void*) {
	parseFile(pCtx, pCtx->args.getString(0).c_str(), true);
}

void ns::registerCommands(Context* pCtx) {
	nsRegisterCommand(pCtx, "echo", 1, 1, echo_command, "prints the passed message to console", "s[message]", "content to print to console");
	nsRegisterCommand(pCtx, "help", 0,1, help_command, "prints a list of commands with their usages or the usage of the specified command", "s[command?]", "command to see usage");
	nsRegisterCommand(pCtx, "var", 1,2, var_command, "creates a variable", "s[name]", "variable name", "s[value?]", "if value is not specified, variable becomes an empty string");
	nsRegisterCommand(pCtx, "delvar", 1,1, delvar_command, "deletes a variable", "v[variable]", "variable to delete");
	nsRegisterCommand(pCtx, "toggle", 3,3, toggle_command, "toggles value between option1 and option2", "s[variableOrCommand]", "variable or command to modify its value", "s[option1]", "option to set variable in case variable value is option2", "s[option2]", "option to set variable in case variable value is option1");
	nsRegisterCommand(pCtx, "exec", 1,1, exec_command, "parses a script file", "s[filePath]", "path to the file");
	nsRegisterCommand(pCtx, "incrementvar", 3,4, incrementvar_command, "do value + delta, when value > max: value = min", "v[variable]", "variable to modify value", "d[min]", "minimum value possible", "d[max]", "maximum possible value", "d[delta?]", "to increase value with -> value + delta");
}

void ns::registerVariable(Context* pCtx, const std::string& name, const std::string& description, void* pVar, const GetProgramVariableValueFn& get, const SetProgramVariableValueFn& set) {
	pCtx->programVariables[name] = ProgramVariable(pVar, description, get, set);
}