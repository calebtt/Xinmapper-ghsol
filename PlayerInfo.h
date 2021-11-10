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
		std::atomic<int> left_dz;
		std::atomic<int> right_dz;
		std::atomic<int> left_trigger_dz;
		std::atomic<int> right_trigger_dz;
		std::atomic<int> player_id;
		PlayerInfo() 
			: left_dz(XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE),
			right_dz(XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE),
			player_id(0),
			left_trigger_dz(XINPUT_GAMEPAD_TRIGGER_THRESHOLD),
			right_trigger_dz(XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
		{}
	};
}