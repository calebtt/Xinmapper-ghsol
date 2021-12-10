#pragma once
#include "stdafx.h"
#include "SensitivityMap.h"
#include "MapFunctions.h"

namespace sds
{
	/// <summary>
	/// Basic logic for mapping thumbstick values to work thread delay values.
	/// A single instance for a single thumbstick axis is to be used.
	/// This class must be re-instantiated to use new deadzone values.
	/// </summary>
	class ThumbstickToDelay
	{
		const std::string BAD_DELAY_MSG = "Bad timer delay value, exception.";
		inline static std::atomic<bool> m_isDeadzoneActivated;
		inline static std::atomic<bool> m_isBothActivated;
		float m_altDeadzoneMultiplier;
		int m_axisSensitivity;
		int m_xAxisDeadzone;
		int m_yAxisDeadzone;
		SensitivityMap m_sensMapper;
		std::map<int, int> m_sharedSensitivityMap;
		//Used to make some assertions about the settings values this class depends upon.
		static void AssertSettings()
		{
			//Assertions about the settings values used by this class.
			if (XinSettings::MICROSECONDS_MIN_MAX >= XinSettings::MICROSECONDS_MAX)
				Utilities::XErrorLogger::LogError("Exception in ThumbstickToDelay() ctor, MICROSECONDS_MIN_MAX >= MICROSECONDS_MAX");
			if (XinSettings::MICROSECONDS_MIN_MAX <= XinSettings::MICROSECONDS_MIN)
				Utilities::XErrorLogger::LogError("Exception in ThumbstickToDelay() ctor, MICROSECONDS_MIN_MAX <= MICROSECONDS_MIN");
			if (XinSettings::MICROSECONDS_MIN >= XinSettings::MICROSECONDS_MAX)
				Utilities::XErrorLogger::LogError("Exception in ThumbstickToDelay() ctor, MICROSECONDS_MIN >= MICROSECONDS_MAX");
			if (XinSettings::SENSITIVITY_MIN >= XinSettings::SENSITIVITY_MAX)
				Utilities::XErrorLogger::LogError("Exception in ThumbstickToDelay() ctor, SENSITIVITY_MIN >= SENSITIVITY_MAX");
			if (XinSettings::SENSITIVITY_MIN <= 0)
				Utilities::XErrorLogger::LogError("Exception in ThumbstickToDelay() ctor, SENSITIVITY_MIN <= 0");
			if (XinSettings::SENSITIVITY_MAX > 100)
				Utilities::XErrorLogger::LogError("Exception in ThumbstickToDelay() ctor, SENSITIVITY_MAX > 100");
		}
		void InitFirstPiece(int sensitivity, int xAxisDz, int yAxisDz, int &outSens, float &outDzMult, int &outDzX, int &outDzY) const
		{
			outSens = RangeBindValue(sensitivity, XinSettings::SENSITIVITY_MIN, XinSettings::SENSITIVITY_MAX);
			outDzMult = XinSettings::ALT_DEADZONE_MULT_DEFAULT;
			outDzX = xAxisDz;
			outDzY = yAxisDz;
		}
		constexpr int RangeBindValue(const int user_sens, const int sens_min, const int sens_max) const
		{
			//bounds check result
			if (user_sens > sens_max)
				return sens_max;
			else if (user_sens < sens_min)
				return sens_min;
			return user_sens;
		}
		constexpr float ToFloat(auto something) const
		{
			return static_cast<float>(something);
		}
		constexpr double ToDub(auto something) const
		{
			return static_cast<double>(something);
		}
		/// <summary>
		/// Alternate main function for use, only considers one axis and one deadzone value.
		/// logs error and returns 1 if bad value internally because this is a function
		/// used in a high precision timer class controlled loop, a bad value would likely mean
		/// a hanging program.
		/// </summary>
		/// <returns>delay in microseconds</returns>
		size_t GetDelayFromThumbstickValue(int val, const bool isX) const
		{
			using namespace Utilities::MapFunctions;
			const int curr = static_cast<int>((!m_isDeadzoneActivated) ?
				(isX ? ToFloat(m_xAxisDeadzone) : ToFloat(m_yAxisDeadzone))
				: (isX ? ToFloat(m_xAxisDeadzone) * m_altDeadzoneMultiplier : ToFloat(m_yAxisDeadzone) * m_altDeadzoneMultiplier));
			val = GetRangedThumbstickValue(val, curr);

			int rval = 0;
			//error checking to make sure the key is in the map
			if (!IsInMap<int, int>(val, m_sharedSensitivityMap, rval))
			{
				//this should not happen, but in case it does I want a plain string telling me it did.
				Utilities::XErrorLogger::LogError("Exception in ThumbstickToDelay::GetDelayFromThumbstickValue(int,bool): " + BAD_DELAY_MSG);
				return 1;
			}

			//double check the value from the map is within bounds
			if (rval >= XinSettings::MICROSECONDS_MIN && rval <= XinSettings::MICROSECONDS_MAX)
				return rval;
			return XinSettings::MICROSECONDS_MAX;
		}
		bool IsBeyondDeadzone(const int val, const bool isX) const
		{
			bool move = 
				(ToFloat(val) > ToFloat(GetDeadzoneCurrent(isX)) 
					|| ToFloat(val) < -ToFloat(GetDeadzoneCurrent(isX)));
			return move;
		}
		int GetDeadzoneCurrent(const bool isX) const
		{
			return static_cast<int>(isX ? (m_isDeadzoneActivated ? ToFloat(m_xAxisDeadzone) * m_altDeadzoneMultiplier : m_xAxisDeadzone) : (m_isDeadzoneActivated ? ToFloat(m_yAxisDeadzone) * m_altDeadzoneMultiplier : m_yAxisDeadzone));
		}
	public:
		/// <summary>
		/// Ctor for dual axis sensitivity and deadzone processing.
		/// Allows getting sensitivity values for the current axis, from using alternate deadzones and sensitivity values for each axis.
		/// In effect, the delay values returned will be influenced by the state of the other axis.
		/// </summary>
		///	<exception cref="std::string"> logs std::string if XinSettings values are unusable. </exception>
		/// <param name="sensitivity">int sensitivity value</param>
		/// <param name="player">PlayerInfo struct full of deadzone information</param>
		/// <param name="whichStick">MouseMap enum denoting which thumbstick</param>
		ThumbstickToDelay(const int sensitivity, const PlayerInfo &player, MouseMap whichStick)
		{
			AssertSettings();
			//error checking mousemap stick setting
			if (whichStick == MouseMap::NEITHER_STICK)
				whichStick = MouseMap::RIGHT_STICK;
			//error checking axisDeadzone arg range, because it might crash the program if the
			//delay returned is some silly value
			int cdx = whichStick == MouseMap::LEFT_STICK ? player.left_x_dz : player.right_x_dz;
			int cdy = whichStick == MouseMap::LEFT_STICK ? player.left_y_dz : player.right_y_dz;
			if (!XinSettings::IsValidDeadzoneValue(cdx))
				cdx = XinSettings::DEADZONE_DEFAULT;
			if (!XinSettings::IsValidDeadzoneValue(cdy))
				cdy = XinSettings::DEADZONE_DEFAULT;
			InitFirstPiece(sensitivity, cdx, cdy, m_axisSensitivity, m_altDeadzoneMultiplier, m_xAxisDeadzone, m_yAxisDeadzone);
			m_isDeadzoneActivated = false;
			m_isBothActivated = false;
			m_sharedSensitivityMap = m_sensMapper.BuildSensitivityMap(m_axisSensitivity,
				XinSettings::SENSITIVITY_MIN,
				XinSettings::SENSITIVITY_MAX,
				XinSettings::MICROSECONDS_MIN,
				XinSettings::MICROSECONDS_MAX,
				XinSettings::MICROSECONDS_MIN_MAX);
		}
		/// <summary>
		/// Ctor for dual axis sensitivity and deadzone processing.
		/// Allows getting high precision timer delay values for the current axis, from using info about each axis.
		/// In effect, the delay values returned will be influenced by the state of the other axis.
		/// </summary>
		///	<exception cref="std::string"> logs std::string if XinSettings values are unusable. </exception>
		/// <param name="sensitivity">is the mouse sensitivity value to use</param>
		/// <param name="xAxisDz">x axis deadzone value</param>
		/// <param name="yAxisDz">y axis deadzone value</param>
		/// <returns></returns>
		ThumbstickToDelay(const int sensitivity, int xAxisDz, int yAxisDz)
		{
			AssertSettings();
			InitFirstPiece(sensitivity, xAxisDz, yAxisDz, m_axisSensitivity, m_altDeadzoneMultiplier, m_xAxisDeadzone, m_yAxisDeadzone);
			m_isDeadzoneActivated = false;
			m_isBothActivated = false;
			//error checking axis dz arg range, because it might crash the program if the
			//delay returned is some silly value
			if (!XinSettings::IsValidDeadzoneValue(xAxisDz))
				m_xAxisDeadzone = XinSettings::DEADZONE_DEFAULT;
			if (!XinSettings::IsValidDeadzoneValue(yAxisDz))
				m_yAxisDeadzone = XinSettings::DEADZONE_DEFAULT;
			m_sharedSensitivityMap = m_sensMapper.BuildSensitivityMap(m_axisSensitivity,
				XinSettings::SENSITIVITY_MIN,
				XinSettings::SENSITIVITY_MAX,
				XinSettings::MICROSECONDS_MIN,
				XinSettings::MICROSECONDS_MAX,
				XinSettings::MICROSECONDS_MIN_MAX);
		}
		ThumbstickToDelay() = delete;
		ThumbstickToDelay(const ThumbstickToDelay& other) = delete;
		ThumbstickToDelay(ThumbstickToDelay&& other) = delete;
		ThumbstickToDelay& operator=(const ThumbstickToDelay& other) = delete;
		ThumbstickToDelay& operator=(ThumbstickToDelay&& other) = delete;
		~ThumbstickToDelay() = default;
		/// <summary>
		/// returns a copy of the internal sensitivity map
		/// </summary>
		/// <returns>std map of int, int</returns>
		[[nodiscard]] std::map<int, int> GetCopyOfSensitivityMap() const
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
			const bool xMove = IsBeyondDeadzone(x, true);
			const bool yMove = IsBeyondDeadzone(y, false);
			if(xMove && yMove)
			{
				m_isBothActivated = true;
			}
			if (!xMove && !yMove)
			{
				m_isDeadzoneActivated = false;
				m_isBothActivated = false;
				return false;
			}
			else if(!xMove || !yMove)
			{
				m_isBothActivated = false;
				return true;
			}
			else
			{
				m_isDeadzoneActivated = true;
				return true;
			}
		}
		/// <summary>
		/// Main function for use, uses information from the other axis
		/// to generate the delay for the current axis.
		/// logs error and returns 1 if bad value internally, because this is a function
		/// used in a high precision timer class controlled loop, a bad value would likely mean
		/// a hanging program.
		/// </summary>
		/// <param name="x">X value</param>
		/// <param name="y">Y value</param>
		/// <param name="isX"> is it the X axis? </param>
		/// <returns>delay in microseconds</returns>
		size_t GetDelayFromThumbstickValue(int x, int y, const bool isX) const
		{
			using namespace Utilities::MapFunctions;
			const int xdz = GetDeadzoneCurrent(true);
			const int ydz = GetDeadzoneCurrent(false);
			x = GetRangedThumbstickValue(x, xdz);
			y = GetRangedThumbstickValue(y, ydz);
			const auto additional = (sqrt(x * x + y * y));
			x = x + static_cast<int>((ToDub(x) / 100.0) * additional);
			y = y + static_cast<int>((ToDub(y) / 100.0) * additional);
			const int totalMagnitude = x + y;
			//Utilities::XErrorLogger::LogError(std::to_string(totalMagnitude));

			int txVal = isX ? x : y;
			//long long txVal = x > y ? x + (0.33f * additional) : y + (0.33f * additional);
			//txVal += static_cast<int>(isX ? 0.33f * ToFloat(x) : 0.33f * ToFloat(y));
			//txVal = TransformSensitivityValue(x, y, isX);

			txVal = RangeBindValue(txVal, XinSettings::SENSITIVITY_MIN, XinSettings::SENSITIVITY_MAX);
			//error checking to make sure the value is in the map
			int rval = 0;
			if (!IsInMap<int, int>(txVal, m_sharedSensitivityMap, rval))
			{
				//this should not happen, but in case it does I want a plain string telling me it did.
				Utilities::XErrorLogger::LogError("Exception in ThumbstickToDelay::GetDelayFromThumbstickValue(int,int,bool): " + BAD_DELAY_MSG);
				return 1;
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
			thumbstick = RangeBindValue(thumbstick, XinSettings::SMin, XinSettings::SMax);
			if (thumbstick == 0)
				return XinSettings::SENSITIVITY_MIN;
			//error checking deadzone arg range
			if (!XinSettings::IsValidDeadzoneValue(axisDeadzone))
				axisDeadzone = XinSettings::DEADZONE_DEFAULT;
			const int absThumb = std::abs(thumbstick);
			if (absThumb < axisDeadzone)
				return XinSettings::SENSITIVITY_MIN;
			int percentage = (absThumb - axisDeadzone) / ((XinSettings::SMax - axisDeadzone) / XinSettings::SENSITIVITY_MAX);
			percentage = RangeBindValue(percentage, XinSettings::SENSITIVITY_MIN, XinSettings::SENSITIVITY_MAX);
			return static_cast<int>(percentage);
		}
	};
}

