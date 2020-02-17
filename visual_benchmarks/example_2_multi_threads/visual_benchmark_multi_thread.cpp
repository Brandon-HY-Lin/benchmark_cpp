
#include <cmath>
#include <thread>
#include <iostream>

#include "instrumentor.h"

#define PROFILING 1

#if PROFILING
#define PROFILE_SCOPE(name) InstrumentationTimer timer##__LINE__(name)
#define PROFILE_FUNCTION() PROFILE_SCOPE(__PRETTY_FUNCTION__ )
#else // ELSE PROFILING
#define PROFILE_SCOPE
#define PROFILE_FUNCTION()
#endif //END PROFILING

namespace Benchmark
{
	constexpr const int N = 1000;

	void PrintFunction(int value)
	{
		PROFILE_FUNCTION();

		for (std::size_t i = 0; i < N; ++i) {
			std::cout << "Hello World #" << (i + value) << std::endl;
		}
	}

	void PrintFunction()
	{
		PROFILE_FUNCTION();

		for (std::size_t i = 0; i < N; ++i) {
			std::cout << "Hello World #" << sqrt(i) << std::endl;
		}
	}

	void RunBenchmarks()
	{
		PROFILE_FUNCTION();

		std::cout << "Running Benchmarks...\n";

		std::thread a([]() {PrintFunction(2); });
		std::thread b([]() {PrintFunction(); });

		a.join();
		b.join();
		
	}
}

int main()
{
	Instrumentor::Get().BeginSession("Profile");
	Benchmark::RunBenchmarks();
	Instrumentor::Get().EndSession();
}