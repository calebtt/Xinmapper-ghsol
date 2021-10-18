#pragma once
#include "stdafx.h"

namespace sds
{
	/// <summary>
	/// ThumbstickAction is a utility class that takes an XINPUT_STATE and and a string token
	/// and returns true if the thumbstick is reported as depressed with regard to the deadzone info in the sds::sdsPlayOne class,
	/// false otherwise. 
	/// The string token is the first and second parts of the ActionDetails type tokens.
	/// Usually "LTHUMB:RIGHT" or similar.
	/// </summary>
	class ThumbstickAction
	{
	public:
		/// <summary>
		/// Utility function that returns true if the thumbstick + direction token's reported value is above the deadzone value
		/// in sdsPlayerInfo. False otherwise.
		/// </summary>
		/// <param name="state"> is an XINPUT_STATE struct with details on the current reported controller state</param>
		/// <param name="token"> is a two-part token containing normally a button and a direction for the thumbsticks,
		/// colon delimited</param>
		/// <returns>true if thumbstick+direction is pressed</returns>
		bool Down(const XINPUT_STATE &state, std::string token)
		{
			//No string in switch, unfortunate.
			std::string temp = sds::sdsActionDescriptors.lThumb + sds::sdsActionDescriptors.moreInfo;
			if( token == temp + sds::sdsActionDescriptors.left )
			{
				//Test lThumb left.
				if( state.Gamepad.sThumbLX < (-sds::sdsPlayerOne.left_dz) )
					return true;
			}
			else if( token == temp + sds::sdsActionDescriptors.right )
			{
				if( state.Gamepad.sThumbLX > sds::sdsPlayerOne.left_dz )
					return true;
			}
			else if( token == temp + sds::sdsActionDescriptors.down )
			{
				if( state.Gamepad.sThumbLY < (-sds::sdsPlayerOne.left_dz) )
					return true;
			}
			else if( token == temp + sds::sdsActionDescriptors.up )
			{
				if( state.Gamepad.sThumbLY > sds::sdsPlayerOne.left_dz )
					return true;
			}
			temp = sds::sdsActionDescriptors.rThumb + sds::sdsActionDescriptors.moreInfo;
			//Right thumbstick.
			if( token == temp + sds::sdsActionDescriptors.left )
			{
				//Test lThumb left.
				if( state.Gamepad.sThumbRX < (-sds::sdsPlayerOne.left_dz) )
					return true;
			}
			else if( token == temp + sds::sdsActionDescriptors.right )
			{
				if( state.Gamepad.sThumbRX > sds::sdsPlayerOne.left_dz )
					return true;
			}
			else if( token == temp + sds::sdsActionDescriptors.down )
			{
				if( state.Gamepad.sThumbRY < (-sds::sdsPlayerOne.left_dz) )
					return true;
			}
			else if( token == temp + sds::sdsActionDescriptors.up )
			{
				if( state.Gamepad.sThumbRY > sds::sdsPlayerOne.left_dz )
					return true;
			}
			return false;
		}
	};

}