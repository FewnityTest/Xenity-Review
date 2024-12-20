// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <chrono>

#include <engine/api.h>

/**
* @brief Class used to test the performances of a piece of code
*/
class API Benchmark
{
public:
	Benchmark() = default;
	Benchmark(const Benchmark& other) = delete;
	Benchmark& operator=(const Benchmark&) = delete;

	/**
	* @brief Start the benchmark
	*/
	void Start();

	/**
	* @brief Calculate the elapsed time since the last Start call
	*/
	void Stop();

	/**
	* @brief Get elapsed microseconds between Start and Stop calls
	*/
	inline uint64_t GetMicroSeconds() const
	{
		return m_time;
	}

	/**
	* @brief Get elapsed milliseconds between Start and Stop calls
	*/
	inline uint64_t GetMilliseconds() const
	{
		return (uint64_t)(m_time / 1000.0f);
	}

	/**
	* @brief Get elapsed seconds between Start and Stop calls
	*/
	inline float GetSeconds() const
	{
		return m_time / 1000000.0f;
	}

	/**
	* @brief Set times values to 0
	*/
	inline void Reset()
	{
		m_time = 0;
	}

private:
	friend class BenchmarkTest;

	std::chrono::time_point<std::chrono::high_resolution_clock> m_start_point, m_end_point;
	uint64_t m_time = 0;

#if defined(__PSP__)
	uint64_t m_endTick;
	uint64_t m_startTick;
#endif
};

