#pragma once
#include <string>

namespace Testy
{
class Report
{
public:
	Report() = delete;
	Report(Report const&) = delete;
	Report(Report&&) = delete;
	void operator=(Report const&) = delete;
	void operator=(Report&&) = delete;

	enum Result
	{
		Passed,
		Failed
	};

	Report(std::string const& testName);

	~Report();

	void Set(Report::Result result, std::string const& resultText);

private:
	std::string m_testName;
	std::string m_resultText = "Success";
	Result m_result = Result::Passed;
};

#define TEST(testName) Testy::Report report(testName)
#define ASSERT(expr, text) \
		if (!(expr)) {\
			report.Set(Testy::Report::Failed, text); \
			return;\
		}


struct DefaultConstructable
{
	DefaultConstructable() = default;
	int value = 0;
};

struct NonTriviallyConstructable
{
	NonTriviallyConstructable() = delete;
	explicit NonTriviallyConstructable(int v) : value(v) {}
	int value = 0;
};

struct OnlyCopyable
{
	explicit OnlyCopyable(int v) : value(v) {}
	OnlyCopyable(OnlyCopyable const&) = default;
	OnlyCopyable& operator=(OnlyCopyable const&) = default;

	OnlyCopyable(OnlyCopyable&&) = delete;
	OnlyCopyable& operator=(OnlyCopyable &&) = delete;

	int value = 0;
};

struct OnlyMovable
{
	explicit OnlyMovable(int v) : value(v) {}
	OnlyMovable(OnlyMovable const&) = delete;
	OnlyMovable& operator=(OnlyMovable const&) = delete;

	OnlyMovable(OnlyMovable &&) = default;
	OnlyMovable& operator=(OnlyMovable &&) = default;

	int value = 0;
};

}