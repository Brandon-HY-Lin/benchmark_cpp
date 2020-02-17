
#include <fstream>
#include <algorithm>
#include <string>
#include <chrono>
#include <cmath>
#include <iostream>

struct ProfileResult
{
	std::string Name;
	long long Start, End;
	uint32_t ThreadID;
};

struct InstrumentationSession
{
	std::string Name;
};

class Instrumentor
{
private:
	InstrumentationSession* m_CurrentSession;
	std::ofstream m_OutputStream;
	int m_ProfileCount;

public:
	Instrumentor()
		: m_CurrentSession(nullptr), m_ProfileCount(0)
	{

	}

	void BeginSession(const std::string& name, const std::string& filepath = "result.json")
	{
		m_OutputStream.open(filepath);

		WriteHeader();

		m_CurrentSession = new InstrumentationSession{ name };


	}

	void EndSession() {
		WriteFooter();

		m_OutputStream.close();

		delete m_CurrentSession;
		m_CurrentSession = nullptr;
		m_ProfileCount = 0;
	}

	void WriteProfile(const ProfileResult& result)
	{
		if (m_ProfileCount++ > 0)
			m_OutputStream << ",";

		std::string name = result.Name;
		std::replace(name.begin(), name.end(), '"', '\'');

		m_OutputStream << "{"
			<< "\"cat\":\"function\","
			<< "\"dur\":" << (result.End - result.Start) << ','
			<< "\"name\":\"" << name << "\","
			<< "\"ph\":\"X\","
			<< "\"pid\":0,"
			<< "\"ts\":" << result.Start
			<< "}";

		m_OutputStream.flush();
	}

	void WriteHeader()
	{
		m_OutputStream << "{\"otherData\": {},\"traceEvents\":[";
		m_OutputStream.flush();
	}

	void WriteFooter()
	{
		m_OutputStream << "]}";
		m_OutputStream.flush();
	}

	static Instrumentor& Get() {
		static Instrumentor instance;
		return instance;
	}
};

class InstrumentationTimer
{
public:
	InstrumentationTimer(const std::string&& name)
		: m_Name(name)
	{
		m_StartTimepoint = std::chrono::high_resolution_clock::now();
	}

	~InstrumentationTimer()
	{
		stop();
	}

	void stop()
	{
		std::chrono::time_point<std::chrono::high_resolution_clock> endTimepoint = std::chrono::high_resolution_clock::now();

		auto start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint)
			.time_since_epoch()
			.count();

		auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint)
			.time_since_epoch()
			.count();

		std::cout << "Start=" << start << ", End=" << end << std::endl;
		// Write elapsed time to file
		Instrumentor::Get().WriteProfile({ m_Name, start, end});
		
	}

private:
	std::string m_Name;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
};

constexpr const std::size_t N = 1000;

void Function1()
{
	{
		InstrumentationTimer timer("Function1");
		for (std::size_t i = 0; i < N; ++i) {
			std::cout << "Hello World #" << i << std::endl;
		}
	}
}

void Function2()
{
	{
		InstrumentationTimer timer("Function2");

		for (std::size_t i = 0; i < N; ++i) {
			std::cout << "Hello World #" << sqrt(i) << std::endl;
		}
	}
}

int main() {
	Instrumentor::Get().BeginSession("Profile");
	Function1();
	Function2();
	Instrumentor::Get().EndSession();
}