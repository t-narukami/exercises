#pragma once

namespace MyAssert_Private 
{
	bool ShowAbortWindow(const char* expr, const char* msg, const char* file, int line);
}

#ifdef _DEBUG

#define _MY_ASSERT_IMPL(expr, msg) \
	do { \
		if (!(expr) && MyAssert_Private::ShowAbortWindow(#expr, msg, __FILE__, __LINE__)) { \
			volatile char* ASSERTION_FAILED = nullptr; \
			*ASSERTION_FAILED = 0; \
		} \
	} while(false);

#define _MY_ASSERT1(expr) _MY_ASSERT_IMPL(expr, "")
#define _MY_ASSERT2(expr, msg) _MY_ASSERT_IMPL(expr, msg)
#define _MY_ASSERTX(_1, _2, NAME, ...) NAME

#define MY_ASSERT(...) _MY_ASSERTX(__VA_ARGS__, _MY_ASSERT2, _MY_ASSERT1)(__VA_ARGS__)

#else
#define MY_ASSERT(expr, msg)
#endif
