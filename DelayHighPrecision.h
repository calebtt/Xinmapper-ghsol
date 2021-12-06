#pragma once
#include "stdafx.h"
#include <chrono>
#include <thread>

namespace sds
{
	namespace DelayHighPrecision
	{
		inline void SleepFor(const std::chrono::microseconds delayValueMicroseconds)
		{
			const auto start = std::chrono::high_resolution_clock::now();
			const auto end = start + delayValueMicroseconds;
			do
			{
				//std::this_thread::yield();
			}
			while (std::chrono::high_resolution_clock::now() < end);
		}
	}
}
