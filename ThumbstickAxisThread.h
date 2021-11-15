#pragma once
#include "stdafx.h"
#include "CPPThreadRunner.h"
#include "SendKey.h"
#include "ThumbstickToDelay.h"

namespace sds
{
    /// <summary>
    /// Spawns a worker thread responsible for sending simulated mouse movements
    /// corresponding to a thumbstick axis.
    /// </summary>
    class ThumbstickAxisThread :
        public CPPThreadRunner<int>
    {
        std::atomic<int> m_sensitivity;
        std::atomic<int> m_deadzone;
        bool m_isInverted; // is axis value translation inverted
		bool m_isX; // x or y axis
		std::shared_ptr<ThumbstickToDelay> m_moveDetermine;
    public:
        ThumbstickAxisThread(int deadzone, int sensitivity, bool isX, bool inverted)
            : CPPThreadRunner<int>(), m_deadzone(deadzone), m_sensitivity(sensitivity), m_isX(isX), m_isInverted(inverted)
        {

        }
        virtual ~ThumbstickAxisThread()
        {

        }
		/// <summary>
		/// Called to update the internal thumbstick value with a new value.
		/// </summary>
		/// <param name="thumbValue"> thumbstick axis value </param>
		void ProcessState(int thumbValue)
		{
			this->updateState(thumbValue);
			

			//if (stickMapInfo == MouseMap::NEITHER_STICK)
			//	return;
			////Holds the reported stick values and will compare to determine if movement has occurred.
			//int tsx, tsy;

			//if (stickMapInfo == MouseMap::RIGHT_STICK)
			//{
			//	tsx = state.Gamepad.sThumbRX;
			//	tsy = state.Gamepad.sThumbRY;
			//}
			//else
			//{
			//	tsx = state.Gamepad.sThumbLX;
			//	tsy = state.Gamepad.sThumbLY;
			//}
			////Give worker thread new values.
			//threadX = tsx;
			//threadY = tsy;

			//int tdz = stickMapInfo == MouseMap::RIGHT_STICK ? sdsPlayerOne.right_dz : sdsPlayerOne.left_dz;
			//ThumbstickToMovement moveDetermine(this->mouseSensitivity, tdz, this->altDeadzoneMultiplier);

			//if (moveDetermine.DoesRequireMove(tsx, tsy))
			//{
			//	//update state.
			//	this->updateState(state);

			//	//check for thread is running.
			//	if (!isThreadRunning)
			//		this->startThread();
			//}
		}
    protected:
        virtual void workThread()
        {
            SendKey keySend;
			m_moveDetermine = std::make_shared<ThumbstickToDelay>(this->m_sensitivity, this->m_deadzone);

            while (!this->isStopRequested)
            {

            }
        }
    };

}
