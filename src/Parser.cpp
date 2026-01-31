#include "Parser.h"

#include <filesystem>
#include <algorithm>
#include <fstream>
#include <sstream>

#include "Print.h"

void ns::clearStatementData(CommandContext* pCtx) {
	pCtx->pCommand = nullptr;
	pCtx->args.arguments.clear();
}

bool ns::canRunVariable(CommandContext* pCtx) {
	switch (pCtx->lexer.token.value[0]) {
	case NS_TOGGLE_ON: {
		ConsoleVariables::pointer pVarPair = &*pCtx->pCtx->consoleVariables.find(pCtx->lexer.token.value);
		auto it = std::find(pCtx->pCtx->toggleVariablesRunning.begin(), pCtx->pCtx->toggleVariablesRunning.end(), pVarPair);

		if (it == pCtx->pCtx->toggleVariablesRunning.end()) {
			pCtx->pCtx->toggleVariablesRunning.push_back(pVarPair);

			return true;
		}

		return false;
	}

	case NS_TOGGLE_OFF: {
		ConsoleVariables::pointer pPlusVariable = nullptr;
		{
			auto it = pCtx->pCtx->consoleVariables.find('+'+pCtx->lexer.token.value.substr(1));
			if (it == pCtx->pCtx->consoleVariables.end())
				return false;

			pPlusVariable = &*it;
		}

		auto it = std::find(pCtx->pCtx->toggleVariablesRunning.begin(), pCtx->pCtx->toggleVariablesRunning.end(), pPlusVariable);
		if (it != pCtx->pCtx->toggleVariablesRunning.end()) {
			pCtx->pCtx->toggleVariablesRunning.erase(it);

			return true;
		}
		return false;
	}

	case NS_LOOP_VARIABLE: {
		ConsoleVariables::pointer pVar = &*pCtx->pCtx->consoleVariables.find(pCtx->lexer.token.value);

		auto it = std::find(pCtx->pCtx->loopVariablesRunning.begin(), pCtx->pCtx->loopVariablesRunning.end(), pVar);
		if (it == pCtx->pCtx->loopVariablesRunning.end())
			pCtx->pCtx->loopVariablesRunning.push_back(pVar);
		else
			pCtx->pCtx->loopVariablesRunning.erase(it);
		return false;
	}

	default:
		return true;
	}
}

void ns::handleCommandCall(CommandContext* pCtx, ProgramVariable*& pProgramVar) {
	if (pProgramVar != nullptr) {
		if (pCtx->args.arguments.size() == 0)
			ns::printf(ns::ECHO, "Value: {}\n{}\n", pProgramVar->get(pCtx, pProgramVar), pProgramVar->description);
		else
			pProgramVar->set(pCtx, pProgramVar, pCtx->args.arguments[0]);

		clearStatementData(pCtx);
		pProgramVar = nullptr;
		return;
	}

	if (pCtx->pCommand == nullptr)
		return;

	if (pCtx->pCommand->minArgs > pCtx->args.arguments.size()) {
		if (pCtx->pCommand->minArgs == pCtx->pCommand->maxArgs)
			ns::printf(ns::ERROR, "Expected {} argument(s) but received {} arguments\n", static_cast<uint16_t>(pCtx->pCommand->minArgs), pCtx->args.arguments.size());
		else
			ns::printf(ns::ERROR, "Expected arguments between [{}, {}] but received {} arguments\n", static_cast<uint16_t>(pCtx->pCommand->minArgs), static_cast<uint16_t>(pCtx->pCommand->maxArgs), pCtx->args.arguments.size());

		ns::printf(ns::ECHO, "{} {}\n", pCtx->pCommand->name, pCtx->pCommand->getArgumentsNames());
		clearStatementData(pCtx);
		return;
	}

	switch (pCtx->pCommand->name[0]) {
	case NS_TOGGLE_ON: {
		auto it = std::find(pCtx->pCtx->toggleCommandsRunning.begin(), pCtx->pCtx->toggleCommandsRunning.end(), pCtx->pCommand);

		if (it == pCtx->pCtx->toggleCommandsRunning.end())
			pCtx->pCtx->toggleCommandsRunning.push_back(pCtx->pCommand);

		else {
			clearStatementData(pCtx);
			return;
		}

		break;
	}

	case NS_TOGGLE_OFF: {
		Command* pPlusCommand = pCtx->pCtx->commands.get('+'+std::string(pCtx->pCommand->name.substr(1)));
		if (pPlusCommand == nullptr)
			break;

		auto it = std::find(pCtx->pCtx->toggleCommandsRunning.begin(), pCtx->pCtx->toggleCommandsRunning.end(), pPlusCommand);

		if (it == pCtx->pCtx->toggleCommandsRunning.end()) {
			clearStatementData(pCtx);
			return;

		} else
			pCtx->pCtx->toggleCommandsRunning.erase(it);

		break;
	}
	}

	pCtx->pCommand->callback(pCtx, pCtx->pCommand->pData);
	clearStatementData(pCtx);
}

ns::TokenFlag ns::handleIdentifierToken(CommandContext* pCtx, ProgramVariable*& pProgramVar, bool printError) {
	if (pCtx->lexer.token.value.empty()) {
		pCtx->lexer.advanceUntil(static_cast<TokenFlag>(TokenType::EOS));
		return 1;
	}

	if (pCtx->pCtx->consoleVariables.count(pCtx->lexer.token.value) != 0) {
		if (canRunVariable(pCtx))
			return 2;

		pCtx->lexer.advanceUntil(static_cast<TokenFlag>(TokenType::EOS));
		return 0;

	} else if (pCtx->pCtx->programVariables.count(pCtx->lexer.token.value) != 0) {
		pProgramVar = &pCtx->pCtx->programVariables[pCtx->lexer.token.value];
		return 1;

	} else {
		pCtx->pCommand = pCtx->pCtx->commands.get(pCtx->lexer.token.value);

		if (pCtx->pCommand == nullptr) {
			if (printError)
				ns::printf(PrintLevel::ERROR, "Unknown identifier \"{}\"\n", pCtx->lexer.token.value);
			pCtx->lexer.advanceUntil(static_cast<TokenFlag>(TokenType::EOS));
			return 0;
		} else
			return 1;
	}

	return false;
}

void ns::handleArgumentToken(CommandContext* pCtx, bool printError) {
	insertReferencesInToken(pCtx, pCtx->lexer.token);

	if (pCtx->lexer.token.value.empty())
		return;

	if (pCtx->pCommand == nullptr) { // if command is nullptr then just append arguments to a single one. This is useful for ProgramVariable
		if (pCtx->args.arguments.size() == 0)
			pCtx->args.arguments.push_back(pCtx->lexer.token.value);
		else
			pCtx->args.arguments.back() += ' '+pCtx->lexer.token.value;
		return;
	}

	if (pCtx->pCommand->maxArgs == 0) {
		if (printError)
			ns::printf(ns::ERROR, "Expected 0 arguments for {} command\n", pCtx->pCommand->name);
		clearStatementData(pCtx);
		pCtx->lexer.advanceUntil(static_cast<TokenFlag>(TokenType::EOS));
		return;
	}

	if (pCtx->args.arguments.size() == pCtx->pCommand->maxArgs) {
		pCtx->lexer.token.value = pCtx->args.arguments.back()+' '+pCtx->lexer.token.value;
		pCtx->args.arguments.pop_back();
	}

	const std::string& arg = pCtx->pCommand->argsDescriptions[pCtx->args.arguments.size()*2];
	switch (arg[0]) {
	case 'i':
		try {
			(void)std::stoll(pCtx->lexer.token.value);
		} catch (...) {
			if (printError)
				ns::printf(PrintLevel::ERROR, "{} -> Type not matched: expected (i)nteger number\n", arg);
			clearStatementData(pCtx);
			pCtx->lexer.advanceUntil(static_cast<TokenFlag>(TokenType::EOS));
			return;
		}
		break;

	case 'd':
		try {
			(void)std::stold(pCtx->lexer.token.value);
		} catch (...) {
			if (printError)
				ns::printf(PrintLevel::ERROR, "{} -> Type not matched: expected (d)ecimal number\n", arg);
			clearStatementData(pCtx);
			pCtx->lexer.advanceUntil(static_cast<TokenFlag>(TokenType::EOS));
			return;
		}
		break;

	case 's':
		break;

	case 'v':
		if (pCtx->pCtx->programVariables.count(pCtx->lexer.token.value) == 0 && pCtx->pCtx->consoleVariables.count(pCtx->lexer.token.value) == 0) {
			if (printError)
				ns::printf(PrintLevel::ERROR, "{} -> Type not matched: expected (v)ariable\n", arg);
			clearStatementData(pCtx);
			pCtx->lexer.advanceUntil(static_cast<TokenFlag>(TokenType::EOS));
			return;
		}
		break;

	default: // should never happen
		break;
	}

	pCtx->args.arguments.push_back(pCtx->lexer.token.value);
}

void ns::handleConsoleVariableCall(CommandContext* pCtx, ProgramVariable*& pProgramVar, bool printError) {
	std::vector<CommandContext*> tempContexts;
	tempContexts.push_back(new CommandContext(pCtx->pCtx, pCtx->pCtx->consoleVariables[pCtx->lexer.token.value]));

	if ((pCtx->origin & OriginType::VARIABLE) != 0)
		tempContexts.back()->origin |= OriginType::VARIABLE_IN_VARIABLE;

	pCtx = tempContexts.back();
	pCtx->lexer.advance();

	pCtx->origin |= OriginType::VARIABLE;

	while (tempContexts.size() != 0) {
		switch (pCtx->lexer.token.type) {
		case TokenType::IDENTIFIER:
			if (handleIdentifierToken(pCtx, pProgramVar, printError) == 2) {
				if (pCtx->pCtx->maxConsoleVariablesRecursiveDepth != 0 && tempContexts.size() >= pCtx->pCtx->maxConsoleVariablesRecursiveDepth) {
					pCtx->lexer.advanceUntil(static_cast<TokenFlag>(TokenType::EOS));
					break;
				}

				tempContexts.push_back(new CommandContext(pCtx->pCtx, pCtx->pCtx->consoleVariables[pCtx->lexer.token.value]));
				pCtx = tempContexts.back();
				pCtx->origin |= OriginType::VARIABLE_IN_VARIABLE;
			}
			break;

		case TokenType::EOS:
			handleCommandCall(pCtx, pProgramVar);
			break;

		case TokenType::ARGUMENT:
			handleArgumentToken(pCtx, printError);
			break;

		default:
			break;
		}

		pCtx->lexer.advance();
		while (pCtx->lexer.token.type == TokenType::END) {
			handleCommandCall(pCtx, pProgramVar);

			delete pCtx;
			tempContexts.pop_back();
			if (tempContexts.size() == 0)
				return;

			pCtx = tempContexts.back();
			pCtx->lexer.advanceUntil(static_cast<TokenFlag>(TokenType::EOS));
		}
	}
}

void ns::updateLoopVariables(Context* pCtx) {
	ns::CommandContext ctx{pCtx};
	ctx.origin |= OriginType::VARIABLE|OriginType::VARIABLE_LOOP;

	for (auto& cVar : pCtx->loopVariablesRunning) {
		ctx.lexer.input = cVar->second;
		parse(&ctx);
		ctx.lexer.clear();
	}
}

void ns::parse(CommandContext* pCtx, bool printError) {
	ProgramVariable* pProgramVar = nullptr;

	pCtx->lexer.advance();
	while (pCtx->lexer.token.type != TokenType::END) {
		switch (pCtx->lexer.token.type) {
		case TokenType::IDENTIFIER: { // can be either variable or command
			TokenFlag result = handleIdentifierToken(pCtx, pProgramVar, printError);
			if (result == 2) {
				handleConsoleVariableCall(pCtx, pProgramVar, printError);
				pCtx->lexer.advanceUntil(static_cast<TokenFlag>(TokenType::EOS));
			} else if (result == 1)
				pCtx->lexer.advance();
			break;
		}

		case TokenType::ARGUMENT:
			handleArgumentToken(pCtx, printError);
			pCtx->lexer.advance();
			break;

		case TokenType::EOS:
			handleCommandCall(pCtx, pProgramVar);
			pCtx->lexer.advance();
			break;

		default:
			pCtx->lexer.advance();
			break;
		}
	}

	handleCommandCall(pCtx, pProgramVar);
}

void ns::parseUntilEOS(CommandContext* pCtx, ProgramVariable*& pProgramVar, bool printError) {
	while ((pCtx->lexer.token.type & (TokenType::EOS|TokenType::END)) == 0) {
		switch (pCtx->lexer.token.type) {
		case TokenType::IDENTIFIER: { // can be either variable or command
			TokenFlag result = handleIdentifierToken(pCtx, pProgramVar, printError);
			if (result == 2) {
				handleConsoleVariableCall(pCtx, pProgramVar, printError);
				pCtx->lexer.advanceUntil(static_cast<TokenFlag>(TokenType::EOS));
			} else if (result == 1)
				pCtx->lexer.advance();
			break;
		}

		case TokenType::ARGUMENT:
			handleArgumentToken(pCtx, printError);
			pCtx->lexer.advance();
			break;

		default:
			pCtx->lexer.advance();
			break;
		}
	}

	handleCommandCall(pCtx, pProgramVar);
	pCtx->lexer.advance();
}

bool ns::parseFile(CommandContext* pCtx, const char* _path, bool printError) {
	CommandContext currentCtx{pCtx->pCtx};
	currentCtx.origin |= pCtx->origin|OriginType::FILE;

	{
		std::filesystem::path path{_path};
		if (!path.has_extension())
			path += NS_FILE_EXTENSION;

		if (!path.has_root_directory())// || _path.parent_path() != NS_ROOT_DIRECTORY)
			path = pCtx->pCtx->cfgDirectory / path;

		currentCtx.filePath = path.string();
	}

	std::ifstream file{currentCtx.filePath};
	if (!file) {
		if (printError)
			printf(PrintLevel::ERROR, "Could not load file \"{}\"\n", currentCtx.filePath);
		return false;
	}

	while (file.good()) {
		std::string line;
		std::getline(file, line);

		// if (!line.empty()) {
		currentCtx.lexer.input += line+'\n';
		// }
	}
	parse(&currentCtx);

	return true;
}