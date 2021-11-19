#pragma once
#include "stdafx.h"

namespace sds
{
	/// <summary>
	/// Utility class that aids in determining if an XINPUT_STATE has the token
	/// information buttons depressed. Only used in "XInputTranslater" class.
	/// </summary>
	class ButtonStateDown
	{
		sds::PlayerInfo localPlayer;
	public:
		ButtonStateDown()
		{
		}
		ButtonStateDown(const sds::PlayerInfo &player)
		{
			localPlayer = player;
		}
		/// <summary>
		/// Utility function that aids in determining if the button is pressed in the XINPUT_STATE
		/// </summary>
		/// <param name="state"> an XINPUT_STATE with current input from the controller</param>
		/// <param name="token"> a string specifying the button info for comparison</param>
		/// <returns>true if the button is depressed, false otherwise</returns>
		bool ButtonDown(const XINPUT_STATE& state, std::string token)
		{
			//std::const_iterator used for access
			for (auto it = sds::sdsActionDescriptors.xin_buttons.cbegin(); it != sds::sdsActionDescriptors.xin_buttons.cend(); ++it)
			{
				if (it->first == token)
				{
					if (state.Gamepad.wButtons & it->second)
						return true;
				}
			}
			return false;
		}

		/// <summary>
		/// Utility function that returns true if the trigger "token" is reported as depressed
		/// by the XINPUT_STATE "state" with regard to deadzone information, false otherwise.
		/// </summary>
		/// <param name="state">is an XINPUT_STATE struct with details on the current reported controller state</param>
		/// <param name="token">is a one-part token containing normally a trigger designation "LTRIGGER" or "RTRIGGER"</param>
		/// <returns>true if trigger is depressed, false otherwise</returns>
		bool TriggerDown(const XINPUT_STATE& state, std::string token)
		{
			if (token == sds::sdsActionDescriptors.lTrigger)
			{
				if (state.Gamepad.bLeftTrigger > sds::sdsPlayerOne.left_trigger_dz)
				{
					return true;
				}
			}
			if (token == sds::sdsActionDescriptors.rTrigger)
			{
				if (state.Gamepad.bRightTrigger > sds::sdsPlayerOne.right_trigger_dz)
				{
					return true;
				}
			}
			return false;
		}

		/// <summary>
		/// Utility function that returns true if the thumbstick + direction token's reported value is above the deadzone value
		/// in sdsPlayerInfo. False otherwise.
		/// </summary>
		/// <param name="state"> is an XINPUT_STATE struct with details on the current reported controller state</param>
		/// <param name="token"> is a two-part token containing normally a button and a direction for the thumbsticks,
		/// colon delimited</param>
		/// <returns>true if thumbstick+direction is pressed</returns>
		bool ThumbstickDown(const XINPUT_STATE& state, std::string token)
		{
			//No string in switch, unfortunate.
			std::string temp = sds::sdsActionDescriptors.lThumb + sds::sdsActionDescriptors.moreInfo;
			if (token == temp + sds::sdsActionDescriptors.left)
			{
				//Test lThumb left.
				if (state.Gamepad.sThumbLX < (-localPlayer.left_x_dz))
					return true;
			}
			else if (token == temp + sds::sdsActionDescriptors.right)
			{
				if (state.Gamepad.sThumbLX > localPlayer.left_x_dz)
					return true;
			}
			else if (token == temp + sds::sdsActionDescriptors.down)
			{
				if (state.Gamepad.sThumbLY < (-localPlayer.left_y_dz))
					return true;
			}
			else if (token == temp + sds::sdsActionDescriptors.up)
			{
				if (state.Gamepad.sThumbLY > localPlayer.left_y_dz)
					return true;
			}
			temp = sds::sdsActionDescriptors.rThumb + sds::sdsActionDescriptors.moreInfo;
			//Right thumbstick.
			if (token == temp + sds::sdsActionDescriptors.left)
			{
				//Test lThumb left.
				if (state.Gamepad.sThumbRX < (-localPlayer.right_x_dz))
					return true;
			}
			else if (token == temp + sds::sdsActionDescriptors.right)
			{
				if (state.Gamepad.sThumbRX > localPlayer.right_x_dz)
					return true;
			}
			else if (token == temp + sds::sdsActionDescriptors.down)
			{
				if (state.Gamepad.sThumbRY < (-localPlayer.right_y_dz))
					return true;
			}
			else if (token == temp + sds::sdsActionDescriptors.up)
			{
				if (state.Gamepad.sThumbRY > localPlayer.right_y_dz)
					return true;
			}
			return false;
		}
	};
}