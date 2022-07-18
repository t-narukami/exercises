#pragma once
#include <string>
#include <unordered_map>

// Benchmarks
namespace Benchy
{

uint64_t GetCPUCycles();

class Report
{
public:
	Report() = default;
	Report(std::string const& name);
	~Report();
	
	void AddBenchmark(std::string const& name, uint64_t cyclesElapsed);

	std::string GetReport() const;

private:
	std::string m_name = "Some benchmark";

	struct Stats
	{
		uint64_t count = 0;
		uint64_t total = 0;
	};
	std::unordered_map<std::string, Stats> m_benchmarks;
};

class Stopwatch
{
public:
	Stopwatch() = delete;
	Stopwatch(Stopwatch const&) = delete;
	Stopwatch& operator=(Stopwatch const&) = delete;

	Stopwatch(Report& report, std::string const& name);
	Stopwatch(Stopwatch&&) noexcept;

	~Stopwatch();
private:
	Report& m_report;
	std::string m_name;
	uint64_t m_start;
};

}