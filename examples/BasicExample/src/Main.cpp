#include <iostream>
#include <string>
#include <stdint.h>

#include <NikiScript/NikiScript.h>
#include <NikiScript/Parser.h>

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


static void test_command(ns::CommandContext* pCtx, void*) {
	// we copy the context so that we can run stuff without changing the main context
	ns::Context contextCopy = ns::deepCopyContext(pCtx->pCtx);

	ns::CommandContext ctx{};
	ctx.pCtx = &contextCopy;

	ctx.lexer = pCtx->args.getString(0);

	ns::parse(&ctx);
}

bool running = false;
static void quit_command(ns::CommandContext*, void*) {
	running = false;
}

bool isCrazy = false;
static void crazy_command(ns::CommandContext* pCtx, void*) {
	if (pCtx->args.arguments.size() == 0) {
		ns::printf(ns::PrintLevel::ECHO, "{}\n", isCrazy);
		return;
	}

	isCrazy = pCtx->args.getSigned<int32_t>(0) > 0;
}


static float floatNumber = 0;
static double doubleNumber = 0;
static long double lDoubleNumber = 0;

static int8_t int8 = 0;
static int16_t int16 = 0;
static int32_t int32 = 0;
static int64_t int64 = 0;

static uint8_t uint8 = 0;
static uint16_t uint16 = 0;
static uint32_t uint32 = 0;
static unsigned long long uint64 = 0;

static uint16_t bits = 0;


void init(ns::Context& ctx) {
	ns::registerCommands(&ctx);

	nsRegisterCommand(&ctx, "quit", 0,1, quit_command, "stops the main loop from running", "s[?]", "");
	nsRegisterCommand(&ctx, "test", 1,1, test_command, "runs script", "s[script]", "parses to nikiscript");
	nsRegisterCommand(&ctx, "crazy", 0,1, crazy_command, "", "i[isCrazy?]", "");

	// decimal numbers
	ns::registerVariable(&ctx, "float", "", &floatNumber, ns::getNumber<float>, ns::setFloat);
	ns::registerVariable(&ctx, "double", "", &doubleNumber, ns::getNumber<double>, ns::setDouble);
	ns::registerVariable(&ctx, "ldouble", "", &lDoubleNumber, ns::getNumber<long double>, ns::setLongDouble);

	// signed numbers
	ns::registerVariable(&ctx, "int8", "", &int8, ns::getNumber<int8_t>, ns::setChar);
	ns::registerVariable(&ctx, "int16", "", &int16, ns::getNumber<int16_t>, ns::setShort);
	ns::registerVariable(&ctx, "int32", "", &int32, ns::getNumber<int32_t>, ns::setInteger);
	ns::registerVariable(&ctx, "int64", "", &int64, ns::getNumber<int64_t>, ns::setLong);

	// unsigned numbers
	ns::registerVariable(&ctx, "uint8", "", &uint8, ns::getNumber<uint8_t>, ns::setUnsigned<uint8_t>);
	ns::registerVariable(&ctx, "uint16", "", &uint16, ns::getNumber<uint16_t>, ns::setUnsigned<uint16_t>);
	ns::registerVariable(&ctx, "uint32", "", &uint32, ns::getNumber<uint32_t>, ns::setUnsigned<uint32_t>);
	ns::registerVariable(&ctx, "uint64", "", &uint64, ns::getNumber<unsigned long long>, ns::setUnsignedLongLong);

	// bits
	ns::registerVariable(&ctx, "bit1", "", &bits, ns::getBit<uint16_t, 1>, ns::setBit<uint16_t, 1>);
	ns::registerVariable(&ctx, "bit2", "", &bits, ns::getBit<uint16_t, 2>, ns::setBit<uint16_t, 2>);
	ns::registerVariable(&ctx, "bit3", "", &bits, ns::getBit<uint16_t, 4>, ns::setBit<uint16_t, 4>);
	ns::registerVariable(&ctx, "bit4", "", &bits, ns::getBit<uint16_t, 8>, ns::setBit<uint16_t, 8>);
	ns::registerVariable(&ctx, "bit5", "", &bits, ns::getBit<uint16_t, 16>, ns::setBit<uint16_t, 16>);
	ns::registerVariable(&ctx, "bit6", "", &bits, ns::getBit<uint16_t, 32>, ns::setBit<uint16_t, 32>);
	ns::registerVariable(&ctx, "bit7", "", &bits, ns::getBit<uint16_t, 64>, ns::setBit<uint16_t, 64>);
	ns::registerVariable(&ctx, "bit8", "", &bits, ns::getBit<uint16_t, 128>, ns::setBit<uint16_t, 128>);
	ns::registerVariable(&ctx, "bit9", "", &bits, ns::getBit<uint16_t, 256>, ns::setBit<uint16_t, 256>);
	ns::registerVariable(&ctx, "bit10", "", &bits, ns::getBit<uint16_t, 512>, ns::setBit<uint16_t, 512>);
	ns::registerVariable(&ctx, "bit11", "", &bits, ns::getBit<uint16_t, 1024>, ns::setBit<uint16_t, 1024>);
	ns::registerVariable(&ctx, "bit12", "", &bits, ns::getBit<uint16_t, 2048>, ns::setBit<uint16_t, 2048>);
	ns::registerVariable(&ctx, "bit13", "", &bits, ns::getBit<uint16_t, 4096>, ns::setBit<uint16_t, 4096>);
	ns::registerVariable(&ctx, "bit14", "", &bits, ns::getBit<uint16_t, 8192>, ns::setBit<uint16_t, 8192>);
	ns::registerVariable(&ctx, "bit15", "", &bits, ns::getBit<uint16_t, 16384>, ns::setBit<uint16_t, 16384>);
	ns::registerVariable(&ctx, "bit16", "", &bits, ns::getBit<uint16_t, 32768>, ns::setBit<uint16_t, 32768>);

	char pName[16] = "nameless tee";
	ns::registerVariable(&ctx, "name", "", pName, ns::getCharArray, ns::setCharArray<16>);
}

int main(int, char**) {
	ns::setPrintCallback(nullptr, nikiScriptPrintCallback);

	ns::Context ctx;
	char _cfgDirectory[] = "data/cfg/";
	ctx.cfgDirectory = _cfgDirectory;
	init(ctx);

	std::string input;

	running = true;
	while (running) {
		input.clear();

		std::cout << "> ";
		std::getline(std::cin, input);

		ns::CommandContext commandCtx;
		commandCtx.pCtx = &ctx;
		commandCtx.lexer = input;
		ns::parse(&commandCtx);

		ns::updateLoopVariables(&ctx);
	}
}