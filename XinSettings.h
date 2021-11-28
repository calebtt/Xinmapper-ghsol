#pragma once
#include "stdafx.h"
#include <mutex>

namespace sds
{
	/// <summary>
	/// Some constants that might someday be configurable, and the functions that help validate associated values.
	/// </summary>
	struct XinSettings
	{
		//Pixels Magnitude is the number of pixels each iteration of the loop
		//sends in ThumbstickAxisThread
		constexpr static const int PIXELS_MAGNITUDE = 1;
		//Pixels Nomove is the number of pixels each iteration of the loop
		//sends in ThumbstickAxisThread for the opposite axis the thread is concerned with.
		constexpr static const int PIXELS_NOMOVE = 0;
		//Thread Delay Micro is the value in microseconds the worker thread in
		//ThumbstickAxisThread sleeps when the previous iteration performed no action.
		constexpr static const int THREAD_DELAY_MICRO = 1500;
		//Input Poller thread delay, in milliseconds.
		constexpr static const int THREAD_DELAY_POLLER = 10;
		//SMax is the value of the Microsoft type "SHORT"'s maximum possible value.
		constexpr static const short SMax = std::numeric_limits<SHORT>::max();
		//SMin is the value of the Microsoft type "SHORT"'s minimum possible value.
		constexpr static const short SMin = std::numeric_limits<SHORT>::min();
		//Sensitivity Min is the minimum mouse sensitivity value allowed, used in several places.
		constexpr static const int SENSITIVITY_MIN = 1;
		//Sensitivity Max is the maximum mouse sensitivity value allowed, used in several places.
		constexpr static const int SENSITIVITY_MAX = 100;
		//Sensitivity Default is the default sensitivity value if none is given.
		constexpr static const int SENSITIVITY_DEFAULT = 35;
		//Microseconds Min is the minimum delay for the thumbstick axis thread loop at the highest thumbstick value.
		constexpr static const int MICROSECONDS_MIN = 500;
		//Microseconds Max is the maximum delay for the thumbstick axis thread loop at the lowest thumbstick value.
		constexpr static const int MICROSECONDS_MAX = 11000;
		//Deadzone Min is the minimum allowable value for a thumbstick deadzone.
		constexpr static const int DEADZONE_MIN = 1;
		//Deadzone Max is the maximum allowable value for a thumbstick deadzone.
		constexpr static const int DEADZONE_MAX = std::numeric_limits<SHORT>::max() - 1;
		//Deadzone Default is the default deadzone value for a thumbstick.
		constexpr static const int DEADZONE_DEFAULT = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
		//Move Thread Sleep Micro is the delay in microseconds for the XInputBoostMouse work thread loop
		//the value determines how often the axis threads are told to process a new state (pair of x,y values).
		constexpr static const int MOVE_THREAD_SLEEP_MICRO = 4000;
		//Multiplier Min is the minimum alt deadzone multiplier value allowed.
		constexpr static const float MULTIPLIER_MIN = 0.01f;
		//Multiplier Max is the maximum alt deadzone multiplier value allowed.
		constexpr static const float MULTIPLIER_MAX = 1.0f;
		//Alt Deadzone Default is the multiplier to use when a deadzone is already activated,
		//the deadzone value for the other axis is lessened via this value.
		constexpr static const float ALT_DEADZONE_MULT_DEFAULT = 0.5f;

		//Static assertions about the const members
		static_assert(SENSITIVITY_MAX < MICROSECONDS_MAX);
		static_assert(SENSITIVITY_MIN >= 1);
		static_assert(SENSITIVITY_MIN < SENSITIVITY_MAX);
		static_assert(DEADZONE_MIN > 0);
		static_assert(DEADZONE_MAX < std::numeric_limits<SHORT>::max());

		static bool IsValidSensitivityValue(int newSens)
		{
			return (newSens <= SENSITIVITY_MAX) && (newSens >= SENSITIVITY_MIN);
		}
		static bool IsValidDeadzoneValue(int dz)
		{
			return (dz <= DEADZONE_MAX) && (dz >= DEADZONE_MIN);
		}
		static bool IsValidThumbstickValue(int thumb)
		{
			return (thumb <= SMax) && (thumb >= SMin);
		}
	};
}

