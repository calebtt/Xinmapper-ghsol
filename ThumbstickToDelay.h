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
		//TODO add alternate deadzone behavior
		inline static bool m_isDeadzoneActivated;
		float m_altDeadzoneMultiplier;
		int m_xAxisDeadzone;
		int m_yAxisDeadzone;
		int m_axisSensitivity;
		const std::string BAD_DELAY_MSG = "Bad timer delay value, exception.";
		

		std::map<int, int> m_sharedSensitivityMap;

		void BuildSensitivityMap()
		{
			int step = XinSettings::MICROSECONDS_MAX / XinSettings::SENSITIVITY_MAX;
			for (int i = XinSettings::SENSITIVITY_MIN, j = XinSettings::SENSITIVITY_MAX; i <= XinSettings::SENSITIVITY_MAX; i++, j--)
			{
				if (i * step < XinSettings::MICROSECONDS_MIN)
				{
					m_sharedSensitivityMap[j] = XinSettings::MICROSECONDS_MIN;
				}
				else
				{
					m_sharedSensitivityMap[j] = i * step;
				}
			}
		}
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
			: m_axisSensitivity(sensitivity), m_altDeadzoneMultiplier(XinSettings::ALT_DEADZONE_MULT_DEFAULT)
		{
			m_isDeadzoneActivated = false;
			m_axisSensitivity = sensitivity;
			m_xAxisDeadzone = whichStick == MouseMap::LEFT_STICK ? player.left_x_dz : player.right_x_dz;
			m_yAxisDeadzone = whichStick == MouseMap::LEFT_STICK ? player.left_y_dz : player.right_y_dz;

			//error checking sensitivity arg range
			if (sensitivity > XinSettings::SENSITIVITY_MAX)
				m_axisSensitivity = XinSettings::SENSITIVITY_MAX;
			else if (sensitivity < XinSettings::SENSITIVITY_MIN)
				m_axisSensitivity = XinSettings::SENSITIVITY_MIN;
			
			//error checking mousemap stick setting
			if (whichStick == MouseMap::NEITHER_STICK)
				whichStick = MouseMap::RIGHT_STICK;

			//error checking axisDeadzone arg range, because it might crash the program if the
			//delay returned is some silly value
			int cdx = whichStick == MouseMap::LEFT_STICK ? player.left_x_dz : player.right_x_dz;
			int cdy = whichStick == MouseMap::LEFT_STICK ? player.left_y_dz : player.right_y_dz;
			if (!XinSettings::IsValidDeadzoneValue(cdx))
				m_xAxisDeadzone = XinSettings::DEADZONE_DEFAULT;
			if (!XinSettings::IsValidDeadzoneValue(cdy))
				m_yAxisDeadzone = XinSettings::DEADZONE_DEFAULT;

			BuildSensitivityMap();
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
			: m_axisSensitivity(sensitivity), m_xAxisDeadzone(xAxisDz), m_yAxisDeadzone(yAxisDz), m_altDeadzoneMultiplier(XinSettings::ALT_DEADZONE_MULT_DEFAULT)
		{
			m_isDeadzoneActivated = false;

			//error checking sensitivity arg range
			if (sensitivity > XinSettings::SENSITIVITY_MAX)
				m_axisSensitivity = XinSettings::SENSITIVITY_MAX;
			else if (sensitivity < XinSettings::SENSITIVITY_MIN)
				m_axisSensitivity = XinSettings::SENSITIVITY_MIN;

			//error checking axis dz arg range, because it might crash the program if the
			//delay returned is some silly value
			if (!XinSettings::IsValidDeadzoneValue(xAxisDz))
				m_xAxisDeadzone = XinSettings::DEADZONE_DEFAULT;
			if (!XinSettings::IsValidDeadzoneValue(yAxisDz))
				m_yAxisDeadzone = XinSettings::DEADZONE_DEFAULT;

			BuildSensitivityMap();
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
			
			if (!xMove && !yMove)
			{
				m_isDeadzoneActivated = false;
				return false;
			}
			else
			{
				m_isDeadzoneActivated = true;
				return true;
			}
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
			int curr = (!m_isDeadzoneActivated) ? (isX ? m_xAxisDeadzone : m_yAxisDeadzone) : (isX ? m_xAxisDeadzone*m_altDeadzoneMultiplier : m_yAxisDeadzone*m_altDeadzoneMultiplier);
			val = GetRangedThumbstickValue( val, curr );
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
			if (rval >= XinSettings::MICROSECONDS_MIN && rval <= XinSettings::MICROSECONDS_MAX)
				return rval;
			return XinSettings::MICROSECONDS_MAX;
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
			
			if (txVal > XinSettings::SENSITIVITY_MAX)
			{
				txVal = XinSettings::SENSITIVITY_MAX;
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

			if (txVal >= XinSettings::MICROSECONDS_MIN && txVal <= XinSettings::MICROSECONDS_MAX)
				return txVal;
			return XinSettings::MICROSECONDS_MAX;
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
			if (thumbstick > XinSettings::SMax)
				thumbstick = XinSettings::SMax;
			else if (thumbstick < XinSettings::SMin)
				thumbstick = XinSettings::SMin;
			else if (thumbstick == 0)
				return XinSettings::SENSITIVITY_MIN;
			//error checking deadzone arg range
			if (!XinSettings::IsValidDeadzoneValue(axisDeadzone))
				axisDeadzone = XinSettings::DEADZONE_DEFAULT;
			int absThumb = std::abs(thumbstick);
			if (absThumb < axisDeadzone)
				return XinSettings::SENSITIVITY_MIN;
			unsigned int percentage = (absThumb - axisDeadzone) / ((XinSettings::SMax - axisDeadzone) / XinSettings::SENSITIVITY_MAX);
			if (percentage < XinSettings::SENSITIVITY_MIN)
				percentage = XinSettings::SENSITIVITY_MIN;
			else if (percentage > XinSettings::SENSITIVITY_MAX)
				percentage = XinSettings::SENSITIVITY_MAX;
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

