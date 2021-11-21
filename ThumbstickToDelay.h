#pragma once
#include "stdafx.h"


namespace sds
{
	/// <summary>
	/// Basic logic for mapping thumbstick values to work thread delay values.
	/// A single instance for a single thumbstick axis is to be used.
	/// This class must be re-instantiated to use new deadzone values.
	/// </summary>
	class ThumbstickToDelay
	{
		int m_xAxisDeadzone;
		int m_yAxisDeadzone;
		int m_axisSensitivity;

		static const short SMax = std::numeric_limits<SHORT>::max();
		static const short SMin = std::numeric_limits<SHORT>::min();
		static const int SENSITIVITY_MIN = 1;
		static const int SENSITIVITY_MAX = 100;
		static const int MICROSECONDS_MIN = 500; //0.5 ms min
		static const int MICROSECONDS_MAX = 10000; //10 ms max
		static const int DEADZONE_MIN = 1;
		constexpr static const int DEADZONE_MAX = std::numeric_limits<SHORT>::max() - 1;
		const std::string BAD_DELAY_MSG = "Bad timer delay value, exception.";
		

		std::map<int, int> m_sharedSensitivityMap;

	public:
		/// <summary>
		/// Ctor for dual axis sensitivity and deadzone processing.
		/// Allows getting sensitivity values for the current axis, from using alternate deadzones and sensitivity values for each axis.
		/// In effect, the delay values returned will be influenced by the state of the other axis.
		/// </summary>
		/// <param name="sensitivity">int sensitivity value</param>
		/// <param name="player">PlayerInfo struct full of deadzone information</param>
		/// <param name="whichStick">MouseMap enum denoting which thumbstick</param>
		ThumbstickToDelay(int sensitivity, const PlayerInfo &player, MouseMap whichStick) noexcept
		{
			//assertions about static const members
			static_assert(SENSITIVITY_MAX < MICROSECONDS_MAX);
			static_assert(SENSITIVITY_MIN >= 1);
			static_assert(SENSITIVITY_MIN < SENSITIVITY_MAX);
			static_assert(DEADZONE_MIN > 0);
			static_assert(DEADZONE_MAX < std::numeric_limits<SHORT>::max());
			
			m_axisSensitivity = sensitivity;
			m_xAxisDeadzone = whichStick == MouseMap::LEFT_STICK ? player.left_x_dz : player.right_x_dz;
			m_yAxisDeadzone = whichStick == MouseMap::LEFT_STICK ? player.left_y_dz : player.right_y_dz;

			//error checking sensitivity arg range
			if (sensitivity > SENSITIVITY_MAX)
				m_axisSensitivity = SENSITIVITY_MAX;
			else if (sensitivity < SENSITIVITY_MIN)
				m_axisSensitivity = SENSITIVITY_MIN;
			
			//error checking mousemap stick setting
			if (whichStick == MouseMap::NEITHER_STICK)
				whichStick = MouseMap::RIGHT_STICK;

			//error checking axisDeadzone arg range, because it might crash the program if the
			//delay returned is some silly value
			int cdx = whichStick == MouseMap::LEFT_STICK ? player.left_x_dz : player.right_x_dz;
			int cdy = whichStick == MouseMap::LEFT_STICK ? player.left_y_dz : player.right_y_dz;

			if (cdx < DEADZONE_MIN)
				m_xAxisDeadzone = DEADZONE_MIN;
			else if (cdx > DEADZONE_MAX)
				m_xAxisDeadzone = DEADZONE_MAX;

			if (cdy < DEADZONE_MIN)
				m_yAxisDeadzone = DEADZONE_MIN;
			else if (cdy > DEADZONE_MAX)
				m_yAxisDeadzone = DEADZONE_MAX;

			int step = MICROSECONDS_MAX / SENSITIVITY_MAX;
			for (int i = SENSITIVITY_MIN, j = SENSITIVITY_MAX; i <= SENSITIVITY_MAX; i++, j--)
			{
				if (i * step < MICROSECONDS_MIN)
				{
					m_sharedSensitivityMap[j] = MICROSECONDS_MIN;
				}
				else
				{
					m_sharedSensitivityMap[j] = i * step;
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
		/// <param name="yAxisDz">y axis deadzone value</param>
		/// <returns></returns>
		ThumbstickToDelay(int sensitivity, int xAxisDz, int yAxisDz) noexcept
			: m_axisSensitivity(sensitivity), m_xAxisDeadzone(xAxisDz), m_yAxisDeadzone(yAxisDz)
		{
			//assertions about static const members
			static_assert(SENSITIVITY_MAX < MICROSECONDS_MAX);
			static_assert(SENSITIVITY_MIN >= 1);
			static_assert(SENSITIVITY_MIN < SENSITIVITY_MAX);
			static_assert(DEADZONE_MIN > 0);
			static_assert(DEADZONE_MAX < std::numeric_limits<SHORT>::max());

			//error checking firstAxisSens arg range
			if (sensitivity > SENSITIVITY_MAX)
				m_axisSensitivity = SENSITIVITY_MAX;
			else if (sensitivity < SENSITIVITY_MIN)
				m_axisSensitivity = SENSITIVITY_MIN;

			//error checking firstAxisDz arg range
			if (xAxisDz < DEADZONE_MIN)
				m_xAxisDeadzone = DEADZONE_MIN;
			else if (xAxisDz > DEADZONE_MAX)
				m_xAxisDeadzone = DEADZONE_MAX;

			//error checking secondAxisDz arg range
			if (yAxisDz < DEADZONE_MIN)
				m_yAxisDeadzone = DEADZONE_MIN;
			else if (yAxisDz > DEADZONE_MAX)
				m_yAxisDeadzone = DEADZONE_MAX;

			int step = MICROSECONDS_MAX / SENSITIVITY_MAX;
			for (int i = SENSITIVITY_MIN, j = SENSITIVITY_MAX; i <= SENSITIVITY_MAX; i++, j--)
			{
				if (i * step < MICROSECONDS_MIN)
				{
					m_sharedSensitivityMap[j] = MICROSECONDS_MIN;
				}
				else
				{
					m_sharedSensitivityMap[j] = i * step;
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
			xMove = (x > m_xAxisDeadzone || x < -m_xAxisDeadzone);
			yMove = (y > m_yAxisDeadzone || y < -m_yAxisDeadzone);
			
			return xMove || yMove;
		}

		/// <summary>
		/// Alternate main function for use, only considers one axis and one deadzone value.
		/// throws std::string if bad value internally because this is a function
		/// used in a high precision timer class controlled loop, a bad value would likely mean
		/// a hanging program.
		/// </summary>
		/// <returns>delay in microseconds</returns>
		size_t GetDelayFromThumbstickValue(int val, bool isX)
		{
			val = GetRangedThumbstickValue( val, (isX ? m_xAxisDeadzone : m_yAxisDeadzone) );
			int rval = 0;
			//error checking to make sure the key is in the map
			auto it = std::find_if(m_sharedSensitivityMap.begin(), m_sharedSensitivityMap.end(), [&val, &rval](const std::pair<int, int> &elem)
				{
					return elem.first == val;
				});
			if (it != m_sharedSensitivityMap.end())
				rval = it->second;
			else
			{
				//this should not happen, but in case it does I want a plain string telling me it did.
				throw std::string("Exception in ThumbstickToDelay::GetDelayFromThumbstickValue(): " + BAD_DELAY_MSG);
			}

			//double check the value from the map is within bounds
			if (rval >= MICROSECONDS_MIN && rval <= MICROSECONDS_MAX)
				return rval;
			return MICROSECONDS_MAX;
		}

		/// <summary>
		/// Main function for use, uses information from the other axis
		/// to generate the delay for the current axis.
		/// throws std::string if bad value internally, because this is a function
		/// used in a high precision timer class controlled loop, a bad value would likely mean
		/// a hanging program.
		/// </summary>
		/// <param name="x">X value</param>
		/// <param name="y">Y value</param>
		/// <param name="isX"> is it the X axis? </param>
		/// <returns>delay in microseconds</returns>
		size_t GetDelayFromThumbstickValue(int x, int y, bool isX)
		{
			x = GetRangedThumbstickValue(x, this->m_xAxisDeadzone);
			y = GetRangedThumbstickValue(y, this->m_yAxisDeadzone);
			
			//add together to lessen the delay, total magnitude of both axes is considered this way
			//long long txVal = static_cast<long long>(x) + static_cast<long long>(y);
			int txVal = 0;
			if (isX)
				txVal = x + (std::sqrt(y)*2);
			else
				txVal = y + (std::sqrt(x)*2);
			
			if (txVal > SENSITIVITY_MAX)
			{
				txVal = SENSITIVITY_MAX;
			}
			//error checking to make sure the value is in the map,
			//and uses the iterator if found to get the mapped value
			auto itx = std::find_if(m_sharedSensitivityMap.begin(), m_sharedSensitivityMap.end(), [&txVal](const std::pair<int, int> &elem)
				{
					return elem.first == txVal;
				});
			if (itx != m_sharedSensitivityMap.end())
			{
				txVal = itx->second;
			}
			else
			{
				//this should not happen, but in case it does I want a plain string telling me it did.
				throw std::string("Exception in ThumbstickToDelay::GetDelayFromThumbstickValue(): " + BAD_DELAY_MSG);
			}

			if (txVal >= MICROSECONDS_MIN && txVal <= MICROSECONDS_MAX)
				return txVal;
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
		/// returns a copy of the internal sensitivity map
		/// </summary>
		/// <returns></returns>
		std::map<int, int> GetCopyOfSensitivityMap() const
		{
			return m_sharedSensitivityMap;
		}


	};
}

