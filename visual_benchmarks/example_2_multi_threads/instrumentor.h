#pragma once

#ifndef INSTRUMENTOR_H
#define INSTRUMENTOR_H

#include <fstream>
#include <algorithm>
#include <chrono>
#include <thread>
#include <iostream>
#include <string>

struct ProfileResult
{
	std::string Name;
	long long Start, End;
	uint32_t ThreadID;
};

class Instrumentor
{
private:
	std::ofstream m_OutputStream;
	std::size_t m_ProfileCount;

public:
	Instrumentor()
		: m_ProfileCount(0)
	{

	}

	void BeginSession(const std::string& name, const std::string& filepath = "results.json")
	{
		m_OutputStream.open(filepath);
		writeHeader();
	}

	void EndSession()
	{
		writeFooter();
		m_OutputStream.close();
		m_ProfileCount = 0;
	}

	void WriteProfile(const ProfileResult& result)
	{
		// append comma before adding a new JSON element.
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
			<< "\"tid\":" << result.ThreadID << ","
			<< "\"ts\":" << result.Start
			<< "}";

		m_OutputStream.flush();
	}


	void writeHeader()
	{
		m_OutputStream << "{\"otherData\": {},\"traceEvents\":[";
		m_OutputStream.flush();
	}

	void writeFooter()
	{
		m_OutputStream << "]}";
		m_OutputStream.flush();
	}

	static Instrumentor& Get()
	{
		static Instrumentor instance;
		return instance;
	}
};


class InstrumentationTimer
{
public:
	InstrumentationTimer(const std::string& name)
		: m_Name(name), m_Stopped(false)
	{
		m_StartTimepoint = std::chrono::high_resolution_clock::now();
	}

	~InstrumentationTimer()
	{
		if (!m_Stopped) {
			stop();
		}
	}

	void stop()
	{
		// record endTimepoint
		std::chrono::time_point<std::chrono::high_resolution_clock> endTimepoint
			= std::chrono::high_resolution_clock::now();

		// convert time_point to double
		long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint)
			.time_since_epoch()
			.count();

		long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint)
			.time_since_epoch()
			.count();

		// Retrieve thread id
		uint32_t threadID = std::hash<std::thread::id>{}(std::this_thread::get_id());


		// write to file
		Instrumentor::Get().WriteProfile({ m_Name, start, end, threadID });

		m_Stopped = true;
	}
private:
	std::string m_Name;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
	bool m_Stopped;
};

#endif // END OF INSTRUMENTOR_H
