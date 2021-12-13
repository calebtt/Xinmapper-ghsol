#pragma once
#include "CPPThreadRunner.h"
#include "SendKey.h"
#include "DelayManager.h"

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
		DelayManager xTime(XinSettings::MICROSECONDS_MAX);
		DelayManager yTime(XinSettings::MICROSECONDS_MAX);
		//A loop with no delay, that checks each delay value
		//against a timepoint, and performs the move for that axis if it beyond the timepoint
		//and in that way, will perform the single pixel move with two different variable time delays.
		bool isXM = false;
		bool isYM = false;
		while(!this->isStopRequested)
		{
			const bool isXPos = m_isXPositive;
			const bool isYPos = m_isYPositive;
			const size_t xDelay = m_xDelay;
			const size_t yDelay = m_yDelay;
			const bool isXPast = xTime();
			const bool isYPast = yTime();
			if (isXM || isYM)
			{
				int xVal = 0;
				int yVal = 0;
				if(isXPast)
				{
					xVal = (isXPos ? XinSettings::PIXELS_MAGNITUDE : (-XinSettings::PIXELS_MAGNITUDE));
					xTime.Reset(xDelay);
				}
				if (isYPast)
				{
					yVal = (isYPos ? -XinSettings::PIXELS_MAGNITUDE : (XinSettings::PIXELS_MAGNITUDE)); // y is inverted
					yTime.Reset(yDelay);
				}
				//TODO there is some kind of condition where at the diagonal and both timers are nearly equal the performance is worse.
				//TODO the vector of movement curves a bit too far. Probably because doesn't use alt deadzone at the moment. Start there.
				keySend.SendMouseMove(xVal, yVal);
			}
			isXM = m_isXMoving;
			isYM = m_isYMoving;
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

		/// <summary>
		/// Called to update mouse mover thread with new microsecond delay values,
		///	and whether the axis to move should move positive or negative.
		/// </summary>
		void UpdateState(const size_t x, const size_t y, const bool isXPositive, const bool isYPositive, const bool isXMoving, const bool isYMoving)
		{
			m_xDelay = x;
			m_yDelay = y;
			m_isXPositive = isXPositive;
			m_isYPositive = isYPositive;
			m_isXMoving = isXMoving;
			m_isYMoving = isYMoving;
		}

	};
}
