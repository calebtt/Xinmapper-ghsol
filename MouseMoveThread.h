#pragma once
#include "CPPThreadRunner.h"
#include "SendKey.h"

namespace sds
{
	/// <summary>
	/// A singular thread responsible for sending mouse movements using
	///	two different axis delay values being updated while running.
	/// </summary>
	class MouseMoveThread : public CPPThreadRunner<int>
	{
		std::atomic<size_t> m_xDelay;
		std::atomic<size_t> m_yDelay;
		std::atomic<bool> m_isXMoving;
		std::atomic<bool> m_isYMoving;
		std::atomic<bool> m_isXPositive;
		std::atomic<bool> m_isYPositive;
	protected:
	void workThread() override
	{
		this->isThreadRunning = true;
		using namespace std::chrono;
		Utilities::SendKey keySend;
		auto xTime = high_resolution_clock::now();
		auto yTime = high_resolution_clock::now();
		//A loop with no delay, that checks each delay value
		//against a timepoint, and performs the move for that axis if it beyond the timepoint
		//and in that way, will perform the single pixel move with two different variable time delays.
		while(!this->isStopRequested)
		{
			const bool isXM = m_isXMoving;
			const bool isYM = m_isYMoving;
			const bool isXP = m_isXPositive;
			const bool isYP = m_isYPositive;
			if (isXM || isYM)
			{
				size_t xDelay = m_xDelay;
				size_t yDelay = m_yDelay;
				//It might be a good idea for a delay value to be "completed" and sent before
				//being changed and processing begins for a new one, testing of this will tell for sure.
				const bool isXPast = high_resolution_clock::now() > xTime + microseconds(xDelay);
				const bool isYPast = high_resolution_clock::now() > yTime + microseconds(yDelay);
				int xVal = (!isXM ? XinSettings::PIXELS_NOMOVE : (isXP? XinSettings::PIXELS_MAGNITUDE : (-XinSettings::PIXELS_MAGNITUDE)));
				int yVal = (!isYM ? XinSettings::PIXELS_NOMOVE : (isYP? XinSettings::PIXELS_MAGNITUDE : (-XinSettings::PIXELS_MAGNITUDE))); // y is inverted before it gets here.
				if (isXPast)
				{
					//xVal = m_isXPositive ? XinSettings::PIXELS_MAGNITUDE : -XinSettings::PIXELS_MAGNITUDE;
					//reset clock
					xTime = high_resolution_clock::now();
				}
				if (isYPast)
				{
					//yVal = m_isYPositive ? XinSettings::PIXELS_MAGNITUDE : -XinSettings::PIXELS_MAGNITUDE;
					//reset clock
					yTime = high_resolution_clock::now();
				}
				if (isXPast || isYPast)
				{
					keySend.SendMouseMove(xVal, yVal);
				}
			}
			else
			{
				//std::this_thread::sleep_for(milliseconds(XinSettings::THREAD_DELAY_POLLER));
			}
		}
		this->isThreadRunning = false;
	}
	public:
		MouseMoveThread() : CPPThreadRunner<int>(), m_xDelay(1), m_yDelay(1), m_isXMoving(false), m_isYMoving(false), m_isXPositive(false), m_isYPositive(false)
		{
			this->startThread();
		}
		~MouseMoveThread() override
		{
			this->stopThread();
		}
		MouseMoveThread(const MouseMoveThread& other) = delete;
		MouseMoveThread(MouseMoveThread&& other) = delete;
		MouseMoveThread& operator=(const MouseMoveThread& other) = delete;
		MouseMoveThread& operator=(MouseMoveThread&& other) = delete;
		void UpdateState(const size_t x, const size_t y, const bool isXPositive, const bool isYPositive, const bool xMoving, const bool yMoving)
		{
			m_xDelay = x;
			m_yDelay = y;
			m_isXPositive = isXPositive;
			m_isYPositive = isYPositive;
			m_isXMoving = xMoving;
			m_isYMoving = yMoving;
		}

	};
}
