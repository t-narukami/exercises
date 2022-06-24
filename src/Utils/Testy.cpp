#include "pch.h"

#include "Testy.h"
#include <iostream>

namespace Testy
{
Report::Report(std::string const& testName)
	: m_testName(testName)
{
}

Report::~Report()
{
	if (m_result == Result::Passed)
	{
		std::cout << "Passed: ";
	}
	else
	{
		std::cout << "Failed: ";
	}
	std::cout << m_testName << " \"" << m_resultText << "\"" << std::endl;
}

void Report::Set(Result result, std::string const& resultText)
{
	m_result = result;
	m_resultText = resultText;
}
}