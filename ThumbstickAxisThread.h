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
        inline static std::mutex m_sendKeyMutex;
        const int PIXELS_MAGNITUDE = 1; //1
        const int PIXELS_NOMOVE = 0; //0
        const int THREAD_DELAY = 1500; //1500 microseconds
        std::atomic<int> m_sensitivity;
        std::atomic<int> m_deadzone;
        std::atomic<int> m_localstate;
        bool m_isX; // x or y axis
        std::shared_ptr<ThumbstickToDelay> m_moveDetermine;
    public:
        ThumbstickAxisThread(int deadzone, int sensitivity, bool isX)
            : CPPThreadRunner<int>(), m_deadzone(deadzone), m_sensitivity(sensitivity), m_isX(isX)
        {

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
            m_localstate = thumbValue;
            if (!this->isThreadRunning && !this->isStopRequested)
            {
                this->startThread();
            }
		}

        /// <summary>
        /// Called to update the internal thumbstick values with new values.
        /// Note the params are not X and Y but rather currentAxis and otherAxis
        /// </summary>
        /// <param name="thumbValue"> thumbstick axis value </param>
        void ProcessState(int thumbValue, int otherAxisValue)
        {
            //TODO complete this function to pass along the values
            m_localstate = thumbValue;
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
        virtual void workThread()
        {
            this->isThreadRunning = true;
            
            //small lambda to ensure no overflow when negating an int
            auto reduceToLongLimit = [](long long valx)
            {
                if (valx <= std::numeric_limits<int>::min())
                    return static_cast<int>(std::numeric_limits<int>::min() + 1);
                else if (valx >= std::numeric_limits<int>::max())
                    return static_cast<int>(std::numeric_limits<int>::max() - 1);
                else
                    return static_cast<int>(valx);
            };
            auto invertIfY = [&reduceToLongLimit](long long &valy, bool isX)
            {
                if (!isX)
                {
                    valy = reduceToLongLimit(valy);
                    valy = -valy;
                }
            };

            using namespace std::chrono;
            SendKey keySend;
			m_moveDetermine = std::make_shared<ThumbstickToDelay>(this->m_sensitivity, this->m_deadzone);
            steady_clock::time_point begin = steady_clock::now();
            steady_clock::time_point end = steady_clock::now();
            auto timeSpan = duration_cast<microseconds>(end - begin);
            bool lastMoved = false;
            long long delayValue = 1;

            //local copies
            long long localVal = m_localstate;
            //invert if Y axis
            invertIfY(localVal, m_isX);

            int movexval = 0;
            int moveyval = 0;
            int testxval = 0;
            int testyval = 0;
            std::shared_ptr<ThumbstickToDelay> thumbDelay = m_moveDetermine;

            //main loop
            while (true)
            {
                if (this->isStopRequested)
                    return;
                
                localVal = m_localstate;
                //invert if Y axis
                invertIfY(localVal, m_isX);

                testxval = m_isX ? static_cast<int>(localVal) : PIXELS_NOMOVE;
                testyval = m_isX ? PIXELS_NOMOVE : static_cast<int>(localVal);
                movexval = m_isX ? (localVal > 0 ? PIXELS_MAGNITUDE : -PIXELS_MAGNITUDE) : PIXELS_NOMOVE;
                moveyval = m_isX ? PIXELS_NOMOVE : (localVal > 0 ? PIXELS_MAGNITUDE : -PIXELS_MAGNITUDE);

                //if last iteration moved the mouse, switch to using the high precision timer for thread delay.
                if (lastMoved)
                {
                    //if timer value greater than last reported delay value
                    if (timeSpan.count() > delayValue)
                    {
                        //reset clock begin
                        //TODO: there is a bug here for super low delay values from GetDelayFromThumbstickValue
                        begin = steady_clock::now();
                        if (thumbDelay->DoesRequireMove(testxval, testyval))
                        {
                            //utilizing the static mutex to synchronize multiple instances of "thumbstickaxisthread"
                            //in that it should alternate between the two instances when required. Without this, a single thread
                            //will "run-away" and send many inputs at once before switching back to the other thread.
                            {
                                lock l(m_sendKeyMutex);
                                keySend.SendMouseMove(movexval, moveyval);
                                lastMoved = true;
                                delayValue = std::abs(std::chrono::microseconds(thumbDelay->GetDelayFromThumbstickValue(static_cast<int>(localVal))).count());
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
                        delayValue = std::abs(std::chrono::microseconds(thumbDelay->GetDelayFromThumbstickValue(static_cast<int>(localVal))).count());
                    }
                }
                else
                {
                    std::this_thread::sleep_for(std::chrono::microseconds(THREAD_DELAY));
                }

                //variable thread delay
                end = steady_clock::now();
                timeSpan = std::chrono::abs(duration_cast<microseconds>(end - begin));
            }
            this->isThreadRunning = false;
        }
    };

}
