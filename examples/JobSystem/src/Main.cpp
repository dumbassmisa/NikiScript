// In this example it's possible to have multiple scripts being parsed.
// It's very simple and it doesn't fully implement the wait command
// correctly. Variables are an example of wait command not working
// as wished. It's possible to make variables work with wait but
// that would require to remake the parse function and this is
// just a simple example.

#include <iostream>
#include <string>
#include <stdint.h>
#include <unordered_map>

#include <NikiScript/NikiScript.h>
#include <NikiScript/Parser.h>

struct Job {
	ns::CommandContext ctx;
	ns::ProgramVariable* pProgramVar = nullptr;
};

const char* printLevelToCStr(ns::PrintLevel level) {
	switch (level) {
	case ns::PrintLevel::ECHO:
		return "ECHO";
	case ns::PrintLevel::WARNING:
		return "WARNING";
	case ns::PrintLevel::ERROR:
		return "ERROR";
	default:
		return "UNKNOWN";
	}
}

void nikiScriptPrintCallback(void*, ns::PrintLevel level, const char* msg) {
	std::cout << printLevelToCStr(level) << ": " << msg;
}


bool running = false;
static void quit_command(ns::CommandContext*, void*) {
	running = false;
}

std::unordered_map<ns::CommandContext*, uint32_t> waitAmounts = {};
static void wait_command(ns::CommandContext* pCtx, void*) {
	if (pCtx->args.arguments.size() == 0)
		waitAmounts[pCtx] = 1;
	else {
		uint16_t cycles = pCtx->args.getUnsigned<uint16_t>(0);
		if (cycles < 1)
			cycles = 1;

		waitAmounts[pCtx] = cycles;
	}
}


int main() {
	ns::setPrintCallback(nullptr, nikiScriptPrintCallback);

	ns::Context ctx;
	ctx.cfgDirectory = "data/cfg/";

	ns::registerCommands(&ctx);

	nsRegisterCommand(&ctx, "quit", 0,1, quit_command, "stops the main loop from running", "s[?]", "");
	nsRegisterCommand(&ctx, "wait", 0,1, wait_command, "wait n cycles before continuing the execution", "i[cycles?]", "how many cycles to wait");

	std::vector<Job*> jobs{};
	std::string input;
	
	running = true;
	while (running) {
		input.clear();

		std::cout << "> ";
		std::getline(std::cin, input);

		{
			Job* pJob = new Job();
			jobs.push_back(pJob);
			pJob->ctx.lexer.input = input;
			pJob->ctx.pCtx = &ctx;
			pJob->ctx.lexer.advance();
		}

		for (size_t i = 0; i < jobs.size();) {
			if (jobs[i]->ctx.lexer.token.type == ns::TokenType::END) {
				delete jobs[i];
				jobs.erase(jobs.begin()+i);
				continue;
			}

			while (waitAmounts.count(&jobs[i]->ctx) == 0 && jobs[i]->ctx.lexer.token.type != ns::TokenType::END)
				ns::parseUntilEOS(&jobs[i]->ctx, jobs[i]->pProgramVar);

			if (waitAmounts.count(&jobs[i]->ctx) != 0) {
				--waitAmounts[&jobs[i]->ctx];
				if (waitAmounts[&jobs[i]->ctx] == 0)
					waitAmounts.erase(&jobs[i]->ctx);
			}

			++i;
		}

		ns::updateLoopVariables(&ctx);
	}
}