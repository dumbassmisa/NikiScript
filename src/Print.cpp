#include "Print.h"

ns::PrintCallbackFn ns::printCallback = nullptr;
void* ns::pPrintCallbackData = nullptr;

void ns::setPrintCallback(void* pData, PrintCallbackFn callback) {
	printCallback = callback;
	pPrintCallbackData = pData;
}

void ns::print(PrintLevel level, const char* str) {
	printCallback(pPrintCallbackData, level, str);
}

void ns::printUnknownCommand(const char* command) {
	printf(PrintLevel::ERROR, "Unknown command \"{}\"\n", command);
}

void ns::printAppendToStringEchoOnly(void* pBuffer, PrintLevel level, const char* string) {
	if (level == PrintLevel::ECHO)
		static_cast<std::string*>(pBuffer)->append(string);
}