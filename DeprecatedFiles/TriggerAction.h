#pragma once
#include "stdafx.h"

namespace sds
{
	/// <summary>
	/// TriggerAction is a utility class for determinining if the triggers are depressed beyond
	/// the deadzone limits. The string token is the first part of the ActionDetails type
	/// tokens. 
	/// Usually "LTRIGGER" or "RTRIGGER"
	/// </summary>
	class TriggerAction
	{
	public:
		/// <summary>
		/// Utility function that returns true if the trigger "token" is reported as depressed
		/// by the XINPUT_STATE "state" with regard to deadzone information, false otherwise.
		/// </summary>
		/// <param name="state">is an XINPUT_STATE struct with details on the current reported controller state</param>
		/// <param name="token">is a one-part token containing normally a trigger designation "LTRIGGER" or "RTRIGGER"</param>
		/// <returns>true if trigger is depressed, false otherwise</returns>
		bool Down(const XINPUT_STATE &state, std::string token)
		{
			if( token == sds::sdsActionDescriptors.lTrigger )
			{
				if( state.Gamepad.bLeftTrigger > sds::sdsPlayerOne.left_trigger_dz )
				{
					return true;
				}
			}
			if( token == sds::sdsActionDescriptors.rTrigger )
			{
				if( state.Gamepad.bRightTrigger > sds::sdsPlayerOne.right_trigger_dz ) 
				{
					return true;
				}
			}
			return false;
		}
	};
}