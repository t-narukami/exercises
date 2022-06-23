#pragma once

namespace MyAssert_Private 
{
	bool ShowAbortWindow(const char* expr, const char* msg, const char* file, int line);
}

#ifdef _DEBUG
#define MY_ASSERT(expr, msg) \
	do { \
		if (!(expr) && MyAssert_Private::ShowAbortWindow(#expr, msg, __FILE__, __LINE__)) { \
			volatile char* ASSERTION_FAILED = nullptr; \
			*ASSERTION_FAILED = 0; \
		} \
	} while(false);
#else
#define MY_ASSERT(expr, msg)
#endif
