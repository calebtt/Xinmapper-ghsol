#pragma once
#include "stdafx.h"


namespace sds
{
	/// <summary>
	/// Basic logic for mapping thumbstick values to work thread delay values.
	/// A single instance for a single thumbstick axis is to be used.
	/// </summary>
	class ThumbstickToDelay
	{
		bool isDeadzoneActivated;
		bool altDeadzoneConfig;

		float altDeadzoneMultiplier;
		int playerDeadzone;
		int mouseSensitivity;

		static const short SMax = std::numeric_limits<SHORT>::max();
		static const short SMin = std::numeric_limits<SHORT>::min();
		static const int SENSITIVITY_MIN = 1;
		static const int SENSITIVITY_MAX = 100;
		static const int MICROSECONDS_MIN = 500; //0.4 ms min
		static const int MICROSECONDS_MAX = 8000; //8 ms max
		static const int DEADZONE_MIN = 1;
		constexpr static const int DEADZONE_MAX = std::numeric_limits<SHORT>::max() - 1;
		

		std::map<int, int> sensitivityMap;

	public:
		//Ctor for not using the alternate deadzone configuration
		ThumbstickToDelay(int sensitivityMouse, int deadzone) noexcept
			: mouseSensitivity(sensitivityMouse), playerDeadzone(deadzone), altDeadzoneConfig(false), altDeadzoneMultiplier(1.0f), isDeadzoneActivated(false)
		{
			//assertions about static const members
			static_assert(SENSITIVITY_MAX < MICROSECONDS_MAX);
			static_assert(SENSITIVITY_MIN >= 1);
			static_assert(SENSITIVITY_MIN < SENSITIVITY_MAX);
			static_assert(DEADZONE_MIN > 0);
			static_assert(DEADZONE_MAX < std::numeric_limits<SHORT>::max());

			//error checking sensitivityMouse arg range
			if (sensitivityMouse > SENSITIVITY_MAX)
				mouseSensitivity = SENSITIVITY_MAX;
			else if (sensitivityMouse < SENSITIVITY_MIN)
				mouseSensitivity = SENSITIVITY_MIN;

			//error checking deadzone arg range
			if (deadzone < DEADZONE_MIN)
				playerDeadzone = DEADZONE_MIN;
			else if (deadzone > DEADZONE_MAX)
				playerDeadzone = DEADZONE_MAX;

			int step = MICROSECONDS_MAX / SENSITIVITY_MAX;
			for (int i = SENSITIVITY_MIN, j = SENSITIVITY_MAX; i <= SENSITIVITY_MAX; i++,j--)
			{
				if (i * step < MICROSECONDS_MIN)
					sensitivityMap[j] = MICROSECONDS_MIN;
				else
					sensitivityMap[j] = i * step;
			}
		}

		/// <summary>
		/// Determines if the X or Y values are greater than the deadzone values and would
		/// thus require movement from the mouse.
		/// </summary>
		/// <param name="x">X value</param>
		/// <param name="y">Y value</param>
		/// <returns>true if requires moving the mouse, false otherwise</returns>
		bool DoesRequireMove(int x, int y)
		{
			bool xMove, yMove;
			xMove = (x > playerDeadzone || x < -playerDeadzone);
			yMove = (y > playerDeadzone || y < -playerDeadzone);

			if (altDeadzoneConfig && isDeadzoneActivated)
			{
				//initial test that will toggle off the isDeadzoneActivated
				//if neither passes the full deadzone value
				if (!xMove && !yMove)
				{
					isDeadzoneActivated = false;
					return false;
				}
				return true;
			}
			else if (xMove || yMove)
			{
				isDeadzoneActivated = true;
				return true;
			}
			else
				return false;
		}

		/// <summary>
		/// Main function for use
		/// throws std::string if bad value internally
		/// </summary>
		/// <returns></returns>
		size_t GetDelayFromThumbstickValue(int val)
		{
			val = GetRangedThumbstickValue(val, this->playerDeadzone);
			int rval = -1;
			std::for_each(sensitivityMap.begin(), sensitivityMap.end(), [&val,&rval](std::pair<const int, int> &elem)
				{
					if (elem.first == val)
						rval = elem.second;
				});
			if (rval >= MICROSECONDS_MIN && rval <= MICROSECONDS_MAX)
				return rval;
			else if (rval == -1)
				throw std::string("Exception in ThumbstickToDelay::GetDelayFromThumbstickValue(): Bad value from GetRangedThumbstickValue");
				//return MICROSECONDS_MAX;
			return MICROSECONDS_MAX;
		}

		/// <summary>
		/// For the case where sensitivity range is 1 to 100
		/// this function will convert the thumbstick value to
		/// an integer percentage. The deadzone value is subtracted before processing.
		/// </summary>
		/// <param name="thumbstick">:thumbstick value between short minimum and short maximum</param>
		/// <param name="axisDeadzone">:positive deadzone value to use for the axis value</param>
		/// <returns>positive value between (inclusive) SENSITIVITY_MIN and SENSITIVITY_MAX, or SENSITIVITY_MIN for thumbstick less than deadzone</returns>
		int GetRangedThumbstickValue(int thumbstick, int axisDeadzone)
		{
			if (thumbstick > SMax)
				thumbstick = SMax;
			else if (thumbstick < SMin)
				thumbstick = SMin;
			else if (thumbstick == 0)
				return SENSITIVITY_MIN;
			//error checking deadzone arg range
			if (axisDeadzone < DEADZONE_MIN)
				axisDeadzone = DEADZONE_MIN;
			else if (axisDeadzone > DEADZONE_MAX)
				axisDeadzone = DEADZONE_MAX;
			int absThumb = std::abs(thumbstick);
			if (absThumb < axisDeadzone)
				return SENSITIVITY_MIN;
			unsigned int percentage = (absThumb - axisDeadzone) / ((SMax - axisDeadzone) / SENSITIVITY_MAX);
			if (percentage < SENSITIVITY_MIN)
				percentage = SENSITIVITY_MIN;
			else if (percentage > SENSITIVITY_MAX)
				percentage = SENSITIVITY_MAX;
			return percentage;
		}
		/// <summary>
		/// Main function for use, uses information from the other axis
		/// to generate the delay for the current axis.
		/// </summary>
		/// <returns></returns>
		size_t GetDelayFromThumbstickValues(int currentAxisVal, int otherAxisVal)
		{
			//TODO return appropriate delay values with information from the other axis
			return 1000;
			long long curVal = 0;
			unsigned long long magnitude = (long long)std::abs(currentAxisVal) + (long long)std::abs(otherAxisVal);
		}

	};
}

