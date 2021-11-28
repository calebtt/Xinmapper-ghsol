#pragma once
#include "stdafx.h"
#include "SensitivityMap.h"

namespace sds
{
	/// <summary>
	/// Basic logic for mapping thumbstick values to work thread delay values.
	/// A single instance for a single thumbstick axis is to be used.
	/// This class must be re-instantiated to use new deadzone values.
	/// </summary>
	///	<exception cref="std::string"> throws string on exception. </exception>
	class ThumbstickToDelay
	{
		inline static std::atomic<bool> m_isDeadzoneActivated;
		float m_altDeadzoneMultiplier;
		int m_xAxisDeadzone;
		int m_yAxisDeadzone;
		int m_axisSensitivity;
		const std::string BAD_DELAY_MSG = "Bad timer delay value, exception.";
		SensitivityMap m_sensMapper;
		std::map<int, int> m_sharedSensitivityMap;
	public:
		/// <summary>
		/// Ctor for dual axis sensitivity and deadzone processing.
		/// Allows getting sensitivity values for the current axis, from using alternate deadzones and sensitivity values for each axis.
		/// In effect, the delay values returned will be influenced by the state of the other axis.
		/// </summary>
		///	<exception cref="std::string"> throws std::string if XinSettings values are unusable. </exception>
		/// <param name="sensitivity">int sensitivity value</param>
		/// <param name="player">PlayerInfo struct full of deadzone information</param>
		/// <param name="whichStick">MouseMap enum denoting which thumbstick</param>
		ThumbstickToDelay(int sensitivity, const PlayerInfo &player, MouseMap whichStick)
		{
			//assertion for the sensitivity function used in this class,
			//if microseconds_min *3 is greater than microseconds_max there is no way to continue
			if (XinSettings::MICROSECONDS_MIN * 3 > XinSettings::MICROSECONDS_MAX)
				throw std::string("Exception in ThumbstickToDelay() ctor, MICROSECONDS_MIN*3 > MICROSECONDS_MAX");

			m_altDeadzoneMultiplier = XinSettings::ALT_DEADZONE_MULT_DEFAULT;
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

			m_sharedSensitivityMap = m_sensMapper.BuildSensitivityMap(sensitivity,
				XinSettings::SENSITIVITY_MIN,
				XinSettings::SENSITIVITY_MAX,
				XinSettings::MICROSECONDS_MIN,
				XinSettings::MICROSECONDS_MAX,
				XinSettings::MICROSECONDS_MIN*3);
		}

		/// <summary>
		/// Ctor for dual axis sensitivity and deadzone processing.
		/// Allows getting high precision timer delay values for the current axis, from using info about each axis.
		/// In effect, the delay values returned will be influenced by the state of the other axis.
		/// </summary>
		///	<exception cref="std::string"> throws std::string if XinSettings values are unusable. </exception>
		/// <param name="sensitivity">is the mouse sensitivity value to use</param>
		/// <param name="xAxisDz">x axis deadzone value</param>
		/// <param name="yAxisDz">y axis deadzone value</param>
		/// <returns></returns>
		ThumbstickToDelay(int sensitivity, int xAxisDz, int yAxisDz)
		{
			//assertion for the sensitivity function used in this class,
			//if microseconds_min *3 is greater than microseconds_max there is no way to continue
			if (XinSettings::MICROSECONDS_MIN * 3 > XinSettings::MICROSECONDS_MAX)
				throw std::string("Exception in ThumbstickToDelay() ctor, MICROSECONDS_MIN*3 > MICROSECONDS_MAX");

			m_axisSensitivity = sensitivity;
			m_xAxisDeadzone = xAxisDz;
			m_yAxisDeadzone = yAxisDz;
			m_altDeadzoneMultiplier = XinSettings::ALT_DEADZONE_MULT_DEFAULT;
			m_isDeadzoneActivated = false;

			//error checking sensitivity arg range
			m_axisSensitivity = RangeBindSensitivityValue(sensitivity, XinSettings::SENSITIVITY_MIN, XinSettings::SENSITIVITY_MAX);

			//error checking axis dz arg range, because it might crash the program if the
			//delay returned is some silly value
			if (!XinSettings::IsValidDeadzoneValue(xAxisDz))
				m_xAxisDeadzone = XinSettings::DEADZONE_DEFAULT;
			if (!XinSettings::IsValidDeadzoneValue(yAxisDz))
				m_yAxisDeadzone = XinSettings::DEADZONE_DEFAULT;

			m_sharedSensitivityMap = m_sensMapper.BuildSensitivityMap(sensitivity,
				XinSettings::SENSITIVITY_MIN,
				XinSettings::SENSITIVITY_MAX,
				XinSettings::MICROSECONDS_MIN,
				XinSettings::MICROSECONDS_MAX,
				XinSettings::MICROSECONDS_MIN * 3);
		}

		/// <summary>
		/// returns a copy of the internal sensitivity map
		/// </summary>
		/// <returns>std map of int, int</returns>
		std::map<int, int> GetCopyOfSensitivityMap() const
		{
			return m_sharedSensitivityMap;
		}

		/// <summary>
		/// Determines if the X or Y values are greater than the deadzone values and would
		/// thus require movement from the mouse.
		/// </summary>
		/// <param name="x">X value</param>
		/// <param name="y">Y value</param>
		/// <returns>true if requires moving the mouse, false otherwise</returns>
		bool DoesRequireMove(const int x, const int y) const
		{
			bool xMove, yMove;
			if(m_isDeadzoneActivated)
			{
				xMove = (x > m_xAxisDeadzone*m_altDeadzoneMultiplier || x < -m_xAxisDeadzone*m_altDeadzoneMultiplier);
				yMove = (y > m_yAxisDeadzone*m_altDeadzoneMultiplier || y < -m_yAxisDeadzone*m_altDeadzoneMultiplier);
			}
			else
			{
				xMove = (x > m_xAxisDeadzone || x < -m_xAxisDeadzone);
				yMove = (y > m_yAxisDeadzone || y < -m_yAxisDeadzone);
			}
			
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
		size_t GetDelayFromThumbstickValue(int val, const bool isX)
		{
			constexpr auto ToFloat = [](auto something) { return static_cast<float>(something); };
			const int curr = static_cast<int>((!m_isDeadzoneActivated) ?
				(isX ? ToFloat(m_xAxisDeadzone) : ToFloat(m_yAxisDeadzone))
				: (isX ? ToFloat(m_xAxisDeadzone) * m_altDeadzoneMultiplier : ToFloat(m_yAxisDeadzone) * m_altDeadzoneMultiplier));
			val = GetRangedThumbstickValue( val, curr );

			int rval = 0;
			//error checking to make sure the key is in the map
			if(!m_sensMapper.IsInMap(val, m_sharedSensitivityMap, rval))
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
		///	<exception cref="std::string"> throws std::string if internal error of computed value outside of sensitivity map range. </exception>
		/// <param name="x">X value</param>
		/// <param name="y">Y value</param>
		/// <param name="isX"> is it the X axis? </param>
		/// <returns>delay in microseconds</returns>
		size_t GetDelayFromThumbstickValue(int x, int y, bool isX)
		{
			constexpr auto ToFloat = [](auto something) { return static_cast<float>(something); };
			const int xdz = static_cast<int>((!m_isDeadzoneActivated) ?
				(isX ? ToFloat(m_xAxisDeadzone) : ToFloat(m_yAxisDeadzone))
				: (isX ? ToFloat(m_xAxisDeadzone) * m_altDeadzoneMultiplier : ToFloat(m_yAxisDeadzone) * m_altDeadzoneMultiplier));
			const int ydz = static_cast<int>((!m_isDeadzoneActivated) ? 
				(!isX ? ToFloat(m_xAxisDeadzone) : ToFloat(m_yAxisDeadzone)) 
				: (!isX ? ToFloat(m_xAxisDeadzone) * m_altDeadzoneMultiplier : ToFloat(m_yAxisDeadzone) * m_altDeadzoneMultiplier));
			x = GetRangedThumbstickValue(x, xdz);
			y = GetRangedThumbstickValue(y, ydz);
			//TODO fix sensitivity bug where the diagonal moves aren't as granular with regard to side to side movements as
			//straight left/right or up/down movements are.

			int txVal = 0;
			txVal = TransformSensitivityValue(x, y, isX);
			txVal = RangeBindSensitivityValue(txVal, XinSettings::SENSITIVITY_MIN, XinSettings::SENSITIVITY_MAX);

			//error checking to make sure the value is in the map
			int rval = 0;
			if (!m_sensMapper.IsInMap(txVal,m_sharedSensitivityMap,rval))
			{
				//this should not happen, but in case it does I want a plain string telling me it did.
				throw std::string("Exception in ThumbstickToDelay::GetDelayFromThumbstickValue(): " + BAD_DELAY_MSG);
			}
			txVal = rval;

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
		int GetRangedThumbstickValue(int thumbstick, int axisDeadzone) const
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
			const int absThumb = std::abs(thumbstick);
			if (absThumb < axisDeadzone)
				return XinSettings::SENSITIVITY_MIN;
			unsigned int percentage = (absThumb - axisDeadzone) / ((XinSettings::SMax - axisDeadzone) / XinSettings::SENSITIVITY_MAX);
			if (percentage < XinSettings::SENSITIVITY_MIN)
				percentage = XinSettings::SENSITIVITY_MIN;
			else if (percentage > XinSettings::SENSITIVITY_MAX)
				percentage = XinSettings::SENSITIVITY_MAX;
			return static_cast<int>(percentage);
		}
	private:
		//The transformation function applied to consider the value of both axes in the calculation.
		int TransformSensitivityValue(int x, int y, bool isX) const
		{
			constexpr auto ToDub = [](auto something) { return static_cast<double>(something); };
			double txVal = XinSettings::SENSITIVITY_MIN;
			if (isX)
				txVal = ToDub(x) + (std::sqrt(y) * 2.0);
			else
				txVal = ToDub(y) + (std::sqrt(x) * 2.0);
			return static_cast<int>(txVal);
		}

		//Keep microsecond delay value within overall min and max
		int RangeBindSensitivityValue(int user_sens, const int sens_min, const int sens_max) const
		{
			//bounds check result
			if (user_sens > sens_max)
				user_sens = sens_max;
			else if (user_sens < sens_min)
				user_sens = sens_min;
			return user_sens;
		}
	};
}

