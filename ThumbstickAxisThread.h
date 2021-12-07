#pragma once
#include "stdafx.h"
#include "CPPThreadRunner.h"
#include "SendKey.h"
#include "ThumbstickToDelay.h"
#include "DelayHighPrecision.h"

namespace sds
{
    /// <summary>
    /// Spawns a worker thread responsible for sending simulated mouse movements
    /// corresponding to a thumbstick axis.
    /// This thread internally inverts the Y axis to better map to mouse movements
    /// as on Windows the coordinate plane will start at the top-left.
    /// </summary>
    class ThumbstickAxisThread
    {
        const bool m_isX; // x or y axis
        std::shared_ptr<ThumbstickToDelay> m_moveDetermine;
    public:
        ThumbstickAxisThread(int sensitivity, const PlayerInfo &player, MouseMap whichStick, const bool isX)
	        : m_isX(isX)
        {
            m_moveDetermine = std::make_shared<ThumbstickToDelay>(sensitivity, player, whichStick);
        }
        ThumbstickAxisThread(const ThumbstickAxisThread& other) = delete;
        ThumbstickAxisThread(ThumbstickAxisThread&& other) = delete;
        ThumbstickAxisThread& operator=(const ThumbstickAxisThread& other) = delete;
        ThumbstickAxisThread& operator=(ThumbstickAxisThread&& other) = delete;
        ~ThumbstickAxisThread() = default;
    	/// <summary>
    	/// Used in conjunction with MouseMoveThread.
    	/// </summary>
    	/// <param name="x"></param>
    	/// <param name="y"></param>
    	/// <returns></returns>
    	size_t GetDelayValue(int x, int y) const
        {
            using namespace std::chrono;
            constexpr auto reduceToIntLimit = [](const int valx) -> int
            {
                if (valx <= std::numeric_limits<int>::min())
                    return static_cast<int>(std::numeric_limits<int>::min() + 1);
                else if (valx >= std::numeric_limits<int>::max())
                    return static_cast<int>(std::numeric_limits<int>::max() - 1);
                else
                    return static_cast<int>(valx);
            };
            //invert if Y axis
            x = reduceToIntLimit(x);
            y = reduceToIntLimit(y);
            y = -y;
        	return std::abs(microseconds(m_moveDetermine->GetDelayFromThumbstickValue(static_cast<int>(x), static_cast<int>(y), m_isX)).count());
        }
        bool DoesRequireMove(const int x, const int y) const
    	{
            return m_moveDetermine->DoesRequireMove(x, y);
    	}
    };

}
