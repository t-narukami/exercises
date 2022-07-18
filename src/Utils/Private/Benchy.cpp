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
	m_benchmarks[name].total += cyclesElapsed;
	++m_benchmarks[name].count;
}

std::string Report::GetReport() const
{
	std::ostringstream report;
	report << m_name << "\n";

	for (auto const& p : m_benchmarks)
	{
		double const cyclesMean = p.second.total / static_cast<double>(p.second.count);
		report << p.first << ":\n\tMean of " << p.second.count << " runs: " << static_cast<uint64_t>(cyclesMean) << " (" << std::scientific << cyclesMean << ") CPU Cycles\n";
	}
	return report.str();
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