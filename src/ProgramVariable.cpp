#include "ProgramVariable.h"

#include "Context.h"
#include "Utils.h"

ns::ProgramVariable::ProgramVariable() {}

ns::ProgramVariable::ProgramVariable(void* pValue, const std::string& description, const GetProgramVariableValueFn& get, const SetProgramVariableValueFn& set)
 : pValue(pValue), description(description), get(get), set(set) {}

std::string ns::getString(CommandContext*, ProgramVariable* pVar) {
	return *static_cast<std::string*>(pVar->pValue);
}

void ns::setString(CommandContext*, ProgramVariable* pVar, const std::string& str) {
	*static_cast<std::string*>(pVar->pValue) = str;
}

std::string ns::getCharArray(CommandContext*, ProgramVariable* pVar) {
	return std::string(static_cast<char*>(pVar->pValue));
}


void ns::setUnsignedLongLong(CommandContext*, ProgramVariable* pVar, const std::string& str) {
	try {
		*static_cast<unsigned long long*>(pVar->pValue) = std::stoull(str);
	} catch (...) {}
}


void ns::setFloat(CommandContext*, ProgramVariable* pVar, const std::string& str) {
	try {
		*static_cast<float*>(pVar->pValue) = std::stof(str);
	} catch (...) {}
}

void ns::setDouble(CommandContext*, ProgramVariable* pVar, const std::string& str) {
	try {
		*static_cast<double*>(pVar->pValue) = std::stod(str);
	} catch (...) {}
}

void ns::setLongDouble(CommandContext*, ProgramVariable* pVar, const std::string& str) {
	try {
		*static_cast<long double*>(pVar->pValue) = std::stold(str);
	} catch (...) {}
}


void ns::setChar(CommandContext*, ProgramVariable* pVar, const std::string& str) {
	try {
		*static_cast<char*>(pVar->pValue) = (char)std::stoi(str);
	} catch (...) {}
}

void ns::setShort(CommandContext*, ProgramVariable* pVar, const std::string& str) {
	try {
		*static_cast<short*>(pVar->pValue) = (short)std::stoi(str);
	} catch (...) {}
}

void ns::setInteger(CommandContext*, ProgramVariable* pVar, const std::string& str) {
	try {
		*static_cast<int*>(pVar->pValue) = std::stoi(str);
	} catch (...) {}
}

void ns::setLong(CommandContext*, ProgramVariable* pVar, const std::string& str) {
	try {
		*static_cast<long*>(pVar->pValue) = std::stol(str);
	} catch (...) {}
}

void ns::setLongLong(CommandContext*, ProgramVariable* pVar, const std::string& str) {
	try {
		*static_cast<long long*>(pVar->pValue) = std::stoll(str);
	} catch (...) {}
}