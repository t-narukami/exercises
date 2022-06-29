#include "pch.h"

#ifndef NO_DEBUG

#include "Assert.h"
#include <sstream>
#include <iostream>

#ifdef _WIN32
#include "Windows.h"

#define AbortMessageBox(message) MessageBoxA(NULL, message, "Assertion Failed", MB_ABORTRETRYIGNORE | MB_ICONERROR | MB_DEFBUTTON1 | MB_SETFOREGROUND)
#define ABORTED IDABORT
#else
#error UNSUPPORTED_PLATFORM
#endif

namespace MyAssert_Private 
{
bool ShowAbortWindow(const char* expr, const char* msg, const char* file, int line)
{
	std::ostringstream ss;
	ss << msg << "\n\"" << expr << "\"\n" << file << ":" << line;
	std::cout << ss.str() << std::endl;
	return AbortMessageBox(ss.str().c_str()) == ABORTED;
}

}

#endif // ifndef NO_DEBUG
