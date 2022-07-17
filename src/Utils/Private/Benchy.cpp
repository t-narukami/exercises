#include "Benchy.h"
#include <iostream>
#include <iomanip>
#include <sstream>

#ifdef _WIN32
#include <intrin.h>
#elif
#include <x86intrin.h>
else
#error UNSUPPORTED_PLATFORM
#endif

namespace Benchy
{

uint64_t GetCPUCycles()
{
	return __rdtsc();
}

Report::Report(std::string const& name)
	: m_name(name)
{
}

Report::~Report()
{
	std::cout << GetReport() << std::endl;
}

void Report::AddBenchmark(std::string const& name, uint64_t cyclesElapsed)
{
	std::ostringstream scientificNotation;
	scientificNotation << std::setw(2) << std::scientific << static_cast<double>(cyclesElapsed);

	std::string const benchmark = name + ":\n\t" + std::to_string(cyclesElapsed) + " (" + scientificNotation.str() + ") CPU Cycles";
	m_benchmarks.push_back(benchmark);
}

std::string Report::GetReport() const
{
	std::string report = m_name + "\n";
	for (std::string const& benchmark : m_benchmarks)
	{
		report.append(benchmark + "\n");
	}
	return report;
}

Stopwatch::Stopwatch(Report& report, std::string const& name)
	: m_report(report)
	, m_name(name)
	, m_start(GetCPUCycles())
{
}

Stopwatch::Stopwatch(Stopwatch&& rhs) noexcept
	: m_report(rhs.m_report)
	, m_name(std::move(rhs.m_name))
	, m_start(std::move(rhs.m_start))
{
}

Stopwatch::~Stopwatch()
{
	uint64_t const cyclesCount = GetCPUCycles() - m_start;
	m_report.AddBenchmark(m_name, cyclesCount);
}

}