#pragma once
#include "stdafx.h"
#include "CPPThreadRunner.h"
#include "SendKey.h"
#include "ThumbstickToDelay.h"
#include "DelayHighPrecision.h"
#include <mutex>
#include <chrono>

namespace sds
{
    /// <summary>
    /// Spawns a worker thread responsible for sending simulated mouse movements
    /// corresponding to a thumbstick axis.
    /// This thread internally inverts the Y axis to better map to mouse movements
    /// as on Windows the coordinate plane will start at the top-left.
    /// </summary>
    class ThumbstickAxisThread :
        public CPPThreadRunner<int>
    {
        sds::PlayerInfo m_localPlayer;
        sds::MouseMap m_localStick;
        std::shared_ptr<ThumbstickToDelay> m_moveDetermine;
        std::atomic<int> m_sensitivity;
        std::atomic<int> m_localX;
        std::atomic<int> m_localY;
        int m_xDeadzone;
        int m_yDeadzone;
        const bool m_isX; // x or y axis
        
    public:
        ThumbstickAxisThread(int sensitivity, const PlayerInfo &player, MouseMap whichStick, const bool isX)
	        : m_isX(isX)
        {
            m_sensitivity = sensitivity;
            m_localPlayer = player;
            m_localStick = whichStick;
            //m_isX = isX;

            if (!XinSettings::IsValidSensitivityValue(sensitivity))
                m_sensitivity = XinSettings::SENSITIVITY_DEFAULT;

            if (whichStick == MouseMap::NEITHER_STICK)
                m_localStick = MouseMap::RIGHT_STICK;

            m_xDeadzone = m_localStick == MouseMap::LEFT_STICK ? player.left_x_dz : player.right_x_dz;
            m_yDeadzone = m_localStick == MouseMap::LEFT_STICK ? player.left_y_dz : player.right_y_dz;
            if (!XinSettings::IsValidDeadzoneValue(m_xDeadzone))
                m_xDeadzone = XinSettings::DEADZONE_DEFAULT;
            if (!XinSettings::IsValidDeadzoneValue(m_yDeadzone))
                m_yDeadzone = XinSettings::DEADZONE_DEFAULT;
            m_localX = 0;
            m_localY = 0;

        }
        ThumbstickAxisThread(const ThumbstickAxisThread& other) = delete;
        ThumbstickAxisThread(ThumbstickAxisThread&& other) = delete;
        ThumbstickAxisThread& operator=(const ThumbstickAxisThread& other) = delete;
        ThumbstickAxisThread& operator=(ThumbstickAxisThread&& other) = delete;
        /// <summary>
        /// Destructor override, ensures the running thread function is stopped
        /// inside of this class and not the base.
        /// </summary>
        ~ThumbstickAxisThread() override
        {
            this->stopThread();
        }
		/// <summary>
		/// Called to update the internal thumbstick value with a new value.
		/// </summary>
		/// <param name="thumbValue"> thumbstick axis value </param>
		void ProcessState(const int thumbValue)
		{
            if (m_isX)
                m_localX = thumbValue;
            else
                m_localY = thumbValue;
            if (!this->isThreadRunning && !this->isStopRequested)
            {
                this->startThread();
            }
		}
        /// <summary>
        /// Called to update the internal thumbstick values with new values.
        /// Note the params are X and Y.
        /// </summary>
        /// <param name="thumbX"> x thumbstick axis value </param>
        /// <param name="thumbY"> y thumbstick axis value </param>
        void ProcessState(const int thumbX, const int thumbY)
        {
            m_localX = thumbX;
            m_localY = thumbY;

            if (!this->isThreadRunning && !this->isStopRequested)
            {
                this->startThread();
            }
        }
        void Start()
        {
            this->startThread();
        }
        void Stop()
        {
            this->stopThread();
        }

    protected:
        constexpr int reduceToLongLimit(const long long valx) const
        {
            if (valx <= std::numeric_limits<int>::min())
                return static_cast<int>(std::numeric_limits<int>::min() + 1);
            else if (valx >= std::numeric_limits<int>::max())
                return static_cast<int>(std::numeric_limits<int>::max() - 1);
            else
                return static_cast<int>(valx);
        }
        constexpr void invertIfY(long long &valy, const bool isX) const
        {
            if (!isX)
            {
                valy = reduceToLongLimit(valy);
                valy = -valy;
            }
        }
        virtual void workThread() override
        {
            this->isThreadRunning = true;
            using namespace std::chrono;
            SendKey keySend;
            m_moveDetermine = std::make_shared<ThumbstickToDelay>(this->m_sensitivity, this->m_localPlayer, this->m_localStick);
            long long delayValue = 1;

            //local copies
            long long localValX = m_localX;
            long long localValY = m_localY;
            //invert if Y axis
            invertIfY(localValX, m_isX);
            invertIfY(localValY, !m_isX);

            int movexval = 0;
            int moveyval = 0;
            int testxval = 0;
            int testyval = 0;
            //main loop
            while (!this->isStopRequested)
            {
                localValX = m_localX;
                localValY = m_localY;
                //invert if Y axis
                invertIfY(localValX, true);
                invertIfY(localValY, false);

                testxval = m_isX ? static_cast<int>(localValX) : XinSettings::PIXELS_NOMOVE;
                testyval = m_isX ? XinSettings::PIXELS_NOMOVE : static_cast<int>(localValY);
                movexval = m_isX ? (localValX > 0 ? XinSettings::PIXELS_MAGNITUDE : -XinSettings::PIXELS_MAGNITUDE) : XinSettings::PIXELS_NOMOVE;
                moveyval = m_isX ? XinSettings::PIXELS_NOMOVE : (localValY > 0 ? XinSettings::PIXELS_MAGNITUDE : -XinSettings::PIXELS_MAGNITUDE);
                delayValue = std::abs(microseconds(XinSettings::MICROSECONDS_MAX).count());
                if (m_moveDetermine->DoesRequireMove(testxval, testyval))
                {
                    keySend.SendMouseMove(movexval, moveyval);
                    //int logDelay = std::abs(microseconds(m_moveDetermine->GetDelayFromThumbstickValue(static_cast<int>(localValX), static_cast<int>(localValY), m_isX)).count());
                    //std::cout << logDelay << std::endl;
                    delayValue = std::abs(microseconds(m_moveDetermine->GetDelayFromThumbstickValue(static_cast<int>(localValX), static_cast<int>(localValY),m_isX)).count());
                }
                //variable thread delay
                sds::DelayHighPrecision::SleepFor(microseconds(delayValue));
            }
            this->isThreadRunning = false;
        }
    };

}
