#include "Context.h"

#ifdef NS_CS_API
size_t ns::Arguments::count() const {
	return arguments.size();
}

uint8_t ns::Arguments::getByte(size_t index) {
	return getUnsigned<uint8_t>(index);
}

uint16_t ns::Arguments::getUInt16(size_t index) {
	return getUnsigned<uint16_t>(index);
}

uint32_t ns::Arguments::getUInt32(size_t index) {
	return getUnsigned<uint32_t>(index);
}

uint64_t ns::Arguments::getUInt64(size_t index) {
	return getUnsigned<uint64_t>(index);
}


int8_t ns::Arguments::getSByte(size_t index) {
	return getUnsigned<int8_t>(index);
}

int16_t ns::Arguments::getInt16(size_t index) {
	return getUnsigned<int16_t>(index);
}

int32_t ns::Arguments::getInt32(size_t index) {
	return getUnsigned<int32_t>(index);
}

int64_t ns::Arguments::getInt64(size_t index) {
	return getUnsigned<int64_t>(index);
}
#endif

NS_CS_UNORDERED_MAP_IMPLEMENTATION(ConsoleVariables, consoleVariables, const std::string&, std::string&, ns::Context::)
NS_CS_UNORDERED_MAP_IMPLEMENTATION(ProgramVariables, programVariables, const std::string&, ns::ProgramVariable&, ns::Context::)
NS_CS_VECTOR_POINTER_PAIR_IMPLEMENTATION(LoopVariablesRunning, loopVariablesRunning, std::string&, const std::string&, ns::Context::);
NS_CS_VECTOR_POINTER_PAIR_IMPLEMENTATION(ToggleVariablesRunning, toggleVariablesRunning, std::string&, const std::string&, ns::Context::);
NS_CS_VECTOR_IMPLEMENTATION(ToggleCommandsRunning, toggleCommandsRunning, ns::Command*, ns::Command*, ns::Context::);

std::string& ns::Arguments::getString(size_t index) {
	return arguments[index];
}


float ns::Arguments::getFloat(size_t index) {
	return std::stof(arguments[index]);
}

double ns::Arguments::getDouble(size_t index) {
	return std::stod(arguments[index]);
}

long double ns::Arguments::getLongDouble(size_t index) {
	return std::stold(arguments[index]);
}


ns::CommandContext::CommandContext() {}
ns::CommandContext::CommandContext(Context* pCtx) : pCtx(pCtx) {}
ns::CommandContext::CommandContext(Context* pCtx, const std::string& lexerInput) : pCtx(pCtx), lexer(lexerInput) {}
ns::CommandContext::CommandContext(Context* pCtx, const char* lexerInput) : pCtx(pCtx), lexer(lexerInput) {}


ns::Context ns::deepCopyContext(const Context* source) {
	Context copy{*source};

	for (size_t i = 0; i < copy.loopVariablesRunning.size(); ++i)
		copy.loopVariablesRunning[i] = &*copy.consoleVariables.find(copy.loopVariablesRunning[i]->first);

	for (size_t i = 0; i < copy.toggleVariablesRunning.size(); ++i)
		copy.toggleVariablesRunning[i] = &*copy.consoleVariables.find(copy.toggleVariablesRunning[i]->first);

	for (size_t i = 0; i < copy.toggleCommandsRunning.size(); ++i)
		copy.toggleCommandsRunning[i] = copy.commands.get(copy.toggleCommandsRunning[i]->name);

	return copy;
}

ns::Origin operator|(ns::OriginType l, ns::OriginType r) {
	return static_cast<ns::Origin>(l)|static_cast<ns::Origin>(r);
}

ns::Origin operator|(ns::Origin l, ns::OriginType r) {
	return l|static_cast<ns::Origin>(r);
}

ns::Origin operator|(ns::OriginType l, ns::Origin r) {
	return static_cast<ns::Origin>(l)|r;
}

ns::Origin& operator|=(ns::Origin& l, ns::OriginType r) {
	l = l|r;
	return l;
}

ns::Origin operator&(ns::Origin l, ns::OriginType r) {
	return l&static_cast<ns::Origin>(r);
}

ns::Origin operator&(ns::OriginType l, ns::Origin r) {
	return static_cast<ns::Origin>(l)&r;
}

ns::Origin& operator&=(ns::Origin& l, ns::OriginType r) {
	l = l & r;
	return l;
}

ns::Origin operator~(ns::OriginType l) {
	return ~static_cast<ns::Origin>(l);
}