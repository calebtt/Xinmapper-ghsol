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
	/// A default constructed PlayerInfo struct has default values that are usable.
	/// </summary>
	struct PlayerInfo
	{
		std::atomic<int> left_x_dz; // left stick X axis dz
		std::atomic<int> left_y_dz; // left stick Y axis dz
		std::atomic<int> right_x_dz; // right stick X axis dz
		std::atomic<int> right_y_dz; // right stick Y axis dz
		std::atomic<int> left_trigger_dz;
		std::atomic<int> right_trigger_dz;
		std::atomic<int> player_id;
		//default ctor
		PlayerInfo() 
			: left_x_dz(XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE),
			left_y_dz(XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE),
			right_x_dz(XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE),
			right_y_dz(XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE),
			left_trigger_dz(XINPUT_GAMEPAD_TRIGGER_THRESHOLD),
			right_trigger_dz(XINPUT_GAMEPAD_TRIGGER_THRESHOLD),
			player_id(0)
		{}
		//copy constructor
		PlayerInfo(const PlayerInfo &sp)
		{
			//temporary copies must be made to copy atomic to atomic
			int ldzx, ldzy, rdzx, rdzy, ltdz, rtdz, pid;
			ldzx = sp.left_x_dz;
			ldzy = sp.left_y_dz;
			rdzx = sp.right_x_dz;
			rdzy = sp.right_y_dz;
			ltdz = sp.left_trigger_dz;
			rtdz = sp.right_trigger_dz;
			pid = sp.player_id;

			left_x_dz = ldzx;
			left_y_dz = ldzy;
			right_x_dz = rdzx;
			right_y_dz = rdzy;
			left_trigger_dz = ltdz;
			right_trigger_dz = rtdz;
			player_id = pid;
		}
		//assignment
		PlayerInfo &operator=(const PlayerInfo &sp)
		{
			if (this == &sp)
				return *this;
			//temporary copies must be made to copy atomic to atomic
			int ldzx, ldzy, rdzx, rdzy, ltdz, rtdz, pid;
			ldzx = sp.left_x_dz;
			ldzy = sp.left_y_dz;
			rdzx = sp.right_x_dz;
			rdzy = sp.right_y_dz;
			ltdz = sp.left_trigger_dz;
			rtdz = sp.right_trigger_dz;
			pid = sp.player_id;
			
			left_x_dz = ldzx;
			left_y_dz = ldzy;
			right_x_dz = rdzx;
			right_y_dz = rdzy;
			left_trigger_dz = ltdz;
			right_trigger_dz = rtdz;
			player_id = pid;

			return *this;
		}
		//move constructor
		PlayerInfo(PlayerInfo &&sp) = delete;
		//Move assignment operator
		PlayerInfo &operator=(PlayerInfo &&sp) = delete;
		~PlayerInfo() = default;
	};
}