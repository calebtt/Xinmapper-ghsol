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
    /// This thread internally inverts the Y axis to better map to mouse movements
    /// as on Windows the coordinate plane will start at the top-left.
    /// </summary>
    class ThumbstickAxisThread :
        public CPPThreadRunner<int>
    {
        inline static std::mutex m_sendKeyMutex; //mutex shared between instances
        sds::PlayerInfo localPlayer;
        sds::MouseMap localStick;
        std::atomic<int> m_sensitivity;
        std::atomic<int> m_localX;
        std::atomic<int> m_localY;
        int m_xDeadzone;
        int m_yDeadzone;
        bool m_isX; // x or y axis
        std::shared_ptr<ThumbstickToDelay> m_moveDetermine;
    public:
        ThumbstickAxisThread(int sensitivity, const PlayerInfo &player, MouseMap whichStick, bool isX)
            : CPPThreadRunner<int>(), m_sensitivity(sensitivity), localPlayer(player), localStick(whichStick), m_isX(isX)
        {
            if (!XinSettings::IsValidSensitivityValue(sensitivity))
                m_sensitivity = XinSettings::SENSITIVITY_DEFAULT;

            if (whichStick == MouseMap::NEITHER_STICK)
                localStick = MouseMap::RIGHT_STICK;

            m_xDeadzone = localStick == MouseMap::LEFT_STICK ? player.left_x_dz : player.right_x_dz;
            m_yDeadzone = localStick == MouseMap::LEFT_STICK ? player.left_y_dz : player.right_y_dz;
            if (!XinSettings::IsValidDeadzoneValue(m_xDeadzone))
                m_xDeadzone = XinSettings::DEADZONE_DEFAULT;
            if (!XinSettings::IsValidDeadzoneValue(m_yDeadzone))
                m_yDeadzone = XinSettings::DEADZONE_DEFAULT;
            m_localX = 0;
            m_localY = 0;

        }
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
		void ProcessState(int thumbValue)
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
        void ProcessState(int thumbX, int thumbY)
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
        int reduceToLongLimit(long long valx)
        {
            if (valx <= std::numeric_limits<int>::min())
                return static_cast<int>(std::numeric_limits<int>::min() + 1);
            else if (valx >= std::numeric_limits<int>::max())
                return static_cast<int>(std::numeric_limits<int>::max() - 1);
            else
                return static_cast<int>(valx);
        }
        void invertIfY(long long &valy, bool isX)
        {
            if (!isX)
            {
                valy = reduceToLongLimit(valy);
                valy = -valy;
            }
        }
        virtual void workThread()
        {
            this->isThreadRunning = true;

            using namespace std::chrono;
            SendKey keySend;
            m_moveDetermine = std::make_shared<ThumbstickToDelay>(this->m_sensitivity, this->localPlayer, this->localStick);
            steady_clock::time_point begin = steady_clock::now();
            steady_clock::time_point end = steady_clock::now();
            auto timeSpan = duration_cast<microseconds>(end - begin);
            bool lastMoved = false;
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
            int deadzone = m_isX ? m_xDeadzone : m_yDeadzone;
            //main loop
            while (true)
            {
                if (this->isStopRequested)
                    return;
                
                localValX = m_localX;
                localValY = m_localY;
                //invert if Y axis
                invertIfY(localValX, true);
                invertIfY(localValY, false);


                testxval = m_isX ? static_cast<int>(localValX) : XinSettings::PIXELS_NOMOVE;
                testyval = m_isX ? XinSettings::PIXELS_NOMOVE : static_cast<int>(localValY);
                movexval = m_isX ? (localValX > 0 ? XinSettings::PIXELS_MAGNITUDE : -XinSettings::PIXELS_MAGNITUDE) : XinSettings::PIXELS_NOMOVE;
                moveyval = m_isX ? XinSettings::PIXELS_NOMOVE : (localValY > 0 ? XinSettings::PIXELS_MAGNITUDE : -XinSettings::PIXELS_MAGNITUDE);
                
                //if last iteration moved the mouse, switch to using the high precision timer for thread delay.
                if (lastMoved)
                {
                    //if timer value greater than last reported delay value
                    if (timeSpan.count() > delayValue)
                    {
                        //reset clock begin
                        begin = steady_clock::now();
                        if (m_moveDetermine->DoesRequireMove(testxval, testyval))
                        {
                            //utilizing the static mutex to synchronize multiple instances of "thumbstickaxisthread"
                            //in that it should alternate between the two instances when required. Without this, a single thread
                            //will "run-away" and send many inputs at once before switching back to the other thread.
                            {
                                lock l(m_sendKeyMutex);
                                keySend.SendMouseMove(movexval, moveyval);
                                lastMoved = true;
                                delayValue = std::abs(std::chrono::microseconds(m_moveDetermine->GetDelayFromThumbstickValue(static_cast<int>(localValX), static_cast<int>(localValY),m_isX)).count());
                            }
                        }
                        else
                            lastMoved = false; // resets lastMoved if no move
                    }
                }
                else if(m_moveDetermine->DoesRequireMove(testxval, testyval) && timeSpan.count() > delayValue)
                {
                    {
                        lock l(m_sendKeyMutex);
                        keySend.SendMouseMove(movexval, moveyval);
                        lastMoved = true;
                        delayValue = std::abs(std::chrono::microseconds(m_moveDetermine->GetDelayFromThumbstickValue(static_cast<int>(localValX), static_cast<int>(localValY), m_isX)).count());
                    }
                }
                else
                {
                    std::this_thread::sleep_for(std::chrono::microseconds(XinSettings::THREAD_DELAY_MICRO));
                }

                //variable thread delay
                end = steady_clock::now();
                timeSpan = std::chrono::abs(duration_cast<microseconds>(end - begin));
            }
            this->isThreadRunning = false;
        }
    };

}
