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
		//bool isAxisDeadzoneActivated;
		//bool isOtherAxisDeadzoneActivated;

		int axisDeadzone;
		int otherAxisDeadzone;
		int axisSensitivity;

		static const short SMax = std::numeric_limits<SHORT>::max();
		static const short SMin = std::numeric_limits<SHORT>::min();
		static const int SENSITIVITY_MIN = 1;
		static const int SENSITIVITY_MAX = 100;
		static const int MICROSECONDS_MIN = 500; //0.5 ms min
		static const int MICROSECONDS_MAX = 8000; //8 ms max
		static const int DEADZONE_MIN = 1;
		constexpr static const int DEADZONE_MAX = std::numeric_limits<SHORT>::max() - 1;
		

		std::map<int, int> sharedSensitivityMap;

	public:
		/// <summary>
		/// Ctor for dual axis sensitivity and deadzone processing.
		/// Allows getting sensitivity values for the current axis, from using alternate deadzones and sensitivity values for each axis.
		/// In effect, the delay values returned will be influenced by the state of the other axis.
		/// </summary>
		/// <param name="firstAxisSens">current axis of interest sensitivity value</param>
		/// <param name="firstAxisDz">current axis of interest deadzone value</param>
		/// <param name="secondAxisSens">other axis sensitivity value</param>
		/// <param name="secondAxisDz">other axis deadzone value</param>
		/// <returns></returns>
		ThumbstickToDelay(int sensitivity, const PlayerInfo &player, MouseMap whichStick) noexcept
		{
			//assertions about static const members
			static_assert(SENSITIVITY_MAX < MICROSECONDS_MAX);
			static_assert(SENSITIVITY_MIN >= 1);
			static_assert(SENSITIVITY_MIN < SENSITIVITY_MAX);
			static_assert(DEADZONE_MIN > 0);
			static_assert(DEADZONE_MAX < std::numeric_limits<SHORT>::max());
			
			axisSensitivity = sensitivity;
			axisDeadzone = whichStick == MouseMap::LEFT_STICK ? player.left_x_dz : player.right_x_dz;
			otherAxisDeadzone = whichStick == MouseMap::LEFT_STICK ? player.left_y_dz : player.right_y_dz;

			//error checking sensitivity arg range
			if (sensitivity > SENSITIVITY_MAX)
				axisSensitivity = SENSITIVITY_MAX;
			else if (sensitivity < SENSITIVITY_MIN)
				axisSensitivity = SENSITIVITY_MIN;
			
			//error checking mousemap stick setting
			if (whichStick == MouseMap::NEITHER_STICK)
				whichStick = MouseMap::RIGHT_STICK;

			//error checking axisDeadzone arg range, because it might crash the program if the
			//delay returned is some silly value
			int cdx = whichStick == MouseMap::LEFT_STICK ? player.left_x_dz : player.right_x_dz;
			int cdy = whichStick == MouseMap::LEFT_STICK ? player.left_y_dz : player.right_y_dz;

			if (cdx < DEADZONE_MIN)
				axisDeadzone = DEADZONE_MIN;
			else if (cdx > DEADZONE_MAX)
				axisDeadzone = DEADZONE_MAX;

			if (cdy < DEADZONE_MIN)
				otherAxisDeadzone = DEADZONE_MIN;
			else if (cdy > DEADZONE_MAX)
				otherAxisDeadzone = DEADZONE_MAX;

			int step = MICROSECONDS_MAX / SENSITIVITY_MAX;
			for (int i = SENSITIVITY_MIN, j = SENSITIVITY_MAX; i <= SENSITIVITY_MAX; i++, j--)
			{
				if (i * step < MICROSECONDS_MIN)
				{
					sharedSensitivityMap[j] = MICROSECONDS_MIN;
				}
				else
				{
					sharedSensitivityMap[j] = i * step;
				}
			}
		}

		/// <summary>
		/// Ctor for dual axis sensitivity and deadzone processing.
		/// Allows getting high precision timer delay values for the current axis, from using info about each axis.
		/// In effect, the delay values returned will be influenced by the state of the other axis.
		/// </summary>
		/// <param name="sensitivity">is the mouse sensitivity value to use</param>
		/// <param name="xAxisDz">x axis deadzone value</param>
		/// <param name="secondAxisDz">y axis deadzone value</param>
		/// <returns></returns>
		ThumbstickToDelay(int sensitivity, int xAxisDz, int yAxisDz) noexcept
			: axisSensitivity(sensitivity), axisDeadzone(xAxisDz), otherAxisDeadzone(yAxisDz)
		{
			//assertions about static const members
			static_assert(SENSITIVITY_MAX < MICROSECONDS_MAX);
			static_assert(SENSITIVITY_MIN >= 1);
			static_assert(SENSITIVITY_MIN < SENSITIVITY_MAX);
			static_assert(DEADZONE_MIN > 0);
			static_assert(DEADZONE_MAX < std::numeric_limits<SHORT>::max());

			//error checking firstAxisSens arg range
			if (sensitivity > SENSITIVITY_MAX)
				axisSensitivity = SENSITIVITY_MAX;
			else if (sensitivity < SENSITIVITY_MIN)
				axisSensitivity = SENSITIVITY_MIN;

			//error checking firstAxisDz arg range
			if (xAxisDz < DEADZONE_MIN)
				axisDeadzone = DEADZONE_MIN;
			else if (xAxisDz > DEADZONE_MAX)
				axisDeadzone = DEADZONE_MAX;

			//error checking secondAxisDz arg range
			if (yAxisDz < DEADZONE_MIN)
				otherAxisDeadzone = DEADZONE_MIN;
			else if (yAxisDz > DEADZONE_MAX)
				otherAxisDeadzone = DEADZONE_MAX;

			int step = MICROSECONDS_MAX / SENSITIVITY_MAX;
			for (int i = SENSITIVITY_MIN, j = SENSITIVITY_MAX; i <= SENSITIVITY_MAX; i++, j--)
			{
				if (i * step < MICROSECONDS_MIN)
				{
					sharedSensitivityMap[j] = MICROSECONDS_MIN;
				}
				else
				{
					sharedSensitivityMap[j] = i * step;
				}
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
			xMove = (x > axisDeadzone || x < -axisDeadzone);
			yMove = (y > otherAxisDeadzone || y < -otherAxisDeadzone);
			
			return xMove || yMove;
		}

		/// <summary>
		/// Alternate main function for use, only considers one axis and one deadzone value.
		/// throws std::string if bad value internally
		/// </summary>
		/// <returns></returns>
		size_t GetDelayFromThumbstickValue(int val)
		{
			val = GetRangedThumbstickValue(val, this->axisDeadzone);
			int rval = -1;
			std::for_each(sharedSensitivityMap.begin(), sharedSensitivityMap.end(), [&val,&rval](std::pair<const int, int> &elem)
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
		/// Main function for use, uses information from the other axis
		/// to generate the delay for the current axis.
		/// throws std::string if bad value internally
		/// </summary>
		/// <returns></returns>
		size_t GetDelayFromThumbstickValues(int currentAxisVal, int otherAxisVal)
		{
			//TODO finish
			currentAxisVal = GetRangedThumbstickValue(currentAxisVal, this->axisDeadzone);
			otherAxisVal = GetRangedThumbstickValue(otherAxisVal, this->axisDeadzone);
			int rval = -1;
			std::for_each(sharedSensitivityMap.begin(), sharedSensitivityMap.end(), [&currentAxisVal, &rval](std::pair<const int, int> &elem)
				{
					if (elem.first == currentAxisVal)
						rval = elem.second;
				});
			if (rval >= MICROSECONDS_MIN && rval <= MICROSECONDS_MAX)
				return rval;
			else if (rval == -1)
				throw std::string("Exception in ThumbstickToDelay::GetDelayFromThumbstickValue(): Bad value from GetRangedThumbstickValue");

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


	};
}

