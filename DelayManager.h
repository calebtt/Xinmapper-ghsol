#pragma once
#include "stdafx.h"

namespace sds
{
	class DelayManager
	{
	public:
		using TimeType = std::chrono::time_point<std::chrono::high_resolution_clock>;
	private:
		TimeType m_startTime;
		size_t m_duration;
		bool m_hasFired;
	public:
		DelayManager(size_t duration_us) : m_startTime(std::chrono::high_resolution_clock::now()), m_duration(duration_us), m_hasFired(false)
		{
		}
		/// <summary>
		/// Check for elapsed.
		/// </summary>
		bool operator()()
		{
			if (std::chrono::high_resolution_clock::now() > (m_startTime + std::chrono::microseconds(m_duration)))
			{
				m_hasFired = true;
				return true;
			}
			return false;
		}
		bool HasFired() const
		{
			return m_hasFired;
		}
		void Reset(size_t newDuration)
		{
			m_startTime = std::chrono::high_resolution_clock::now();
			m_hasFired = false;
			m_duration = newDuration;
		}
	};
}

