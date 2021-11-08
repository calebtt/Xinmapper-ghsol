/*
Holds program information for the specific player,
like deadzone information.
*/
#pragma once
namespace sds
{
	/// <summary>
	/// Used as a global data structure to hold player information, includes
	/// thumbstick and trigger deadzone information.
	/// </summary>
	struct PlayerInfo
	{
		int left_dz;
		int right_dz;
		int left_trigger_dz;
		int right_trigger_dz;
		int player_id;
		PlayerInfo() 
			: left_dz(XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE),
			right_dz(XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE),
			player_id(0),
			left_trigger_dz(XINPUT_GAMEPAD_TRIGGER_THRESHOLD),
			right_trigger_dz(XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
		{}
	};
}