#pragma once
#include "stdafx.h"

namespace sds
{
	/// <summary>
	/// ActionDescriptors is a big structure full of keywords that are used by other classes to enable
	/// processing of an sds::ActionDetails string into meaningful information for the program.
	/// It has a map&lt;string,int&gt; named "xin_buttons" that is very useful for mapping the string into XINPUT #defines
	/// 
	/// The Xbox 360 controller keyboard's keys are mapped by default when using GamepadKeyboardUser,
	/// even if they are not in the map string!
	/// </summary>
	struct ActionDescriptors
	{
		const char moreInfo = ':'; // the char ':'
		const char delimiter = ' ';//spacebar space

		//using this declaration syntax gives intellisense the comments per variable.
		const std::string x = "X"; // the string "X"
		const std::string y = "Y"; // the string "Y"
		const std::string a = "A"; // the string "A"
		const std::string b = "B"; // the string "B"
		const std::string lThumb = "LTHUMB"; // the string "LTHUMB"
		const std::string rThumb = "RTHUMB"; // the string "RTHUMB"
		const std::string lTrigger = "LTRIGGER"; // the string "LTRIGGER"
		const std::string rTrigger = "RTRIGGER"; // the string "RTRIGGER"
		const std::string lShoulder = "LSHOULDER"; // the string "LSHOULDER"
		const std::string rShoulder = "RSHOULDER"; // the string "RSHOULDER"
		const std::string dpad = "DPAD"; // the string "DPAD"
		const std::string left = "LEFT"; // the string "LEFT"
		const std::string right = "RIGHT"; // the string "RIGHT"
		const std::string up = "UP"; // the string "UP"
		const std::string down = "DOWN"; // the string "DOWN"

		const std::string upleft = "UPLEFT"; // the string "UPLEFT"
		const std::string upright = "UPRIGHT"; // the string "UPRIGHT"
		const std::string downright = "DOWNRIGHT"; // the string "DOWNRIGHT"
		const std::string downleft = "DOWNLEFT"; // the string "DOWNLEFT"

		const std::string none = "NONE"; // the string "NONE"
		const std::string start = "START"; // the string "START"
		const std::string back = "BACK"; // the string "BACK"
		const std::string vk = "VK"; // the string "VK"
		const std::string mappedTo = "="; // the string "="
		const std::string norm = "NORM"; // the string "NORM"
		const std::string toggle = "TOGGLE"; // the string "TOGGLE"
		const std::string rapid = "RAPID"; // the string "RAPID"



		/// <summary>
		/// Maps the tokens above to XINPUT library macro defines.
		/// Because the XINPUT lib doesn't send a "down" signal
		/// to be tested against with a define bitmask, it isn't
		/// mapped to an xinput lib define bitmask here.
		/// instead, lTrigger, rTrigger are tested against the current value
		/// in BYTE bLeftTrigger and bRightTrigger in the XINPUT_GAMEPAD struct.
		/// XInputGetKeyState() cannot be used for Xbox360 controller keyboard keys.
		/// </summary>
		const std::map<const std::string, int> xin_buttons = 
		{
			{x,XINPUT_GAMEPAD_X},
			{y,XINPUT_GAMEPAD_Y},
			{a,XINPUT_GAMEPAD_A},
			{b,XINPUT_GAMEPAD_B},
			{lShoulder,XINPUT_GAMEPAD_LEFT_SHOULDER},
			{rShoulder,XINPUT_GAMEPAD_RIGHT_SHOULDER},
			{dpad + moreInfo + left, XINPUT_GAMEPAD_DPAD_LEFT},
			{dpad + moreInfo + right, XINPUT_GAMEPAD_DPAD_RIGHT},
			{dpad + moreInfo + up, XINPUT_GAMEPAD_DPAD_UP},
			{dpad + moreInfo + down, XINPUT_GAMEPAD_DPAD_DOWN},
			{start, XINPUT_GAMEPAD_START},
			{back, XINPUT_GAMEPAD_BACK},
			{lThumb, XINPUT_GAMEPAD_LEFT_THUMB},
			{rThumb, XINPUT_GAMEPAD_RIGHT_THUMB}
		};

		/// <summary>
		/// Map for the named macro defines in the XInputGetKeystroke() lib.
		/// Key presses (from the xbox360 controller keyboard) do not have
		/// separate macro defines, but instead report a virtual keycode.
		/// Also note that the thumbstick directions are not handled here,
		/// though they do have macro defines. They report only the direction
		/// that the stick is moved, and not a magnitude value as if by the
		/// older XINPUT GetKeyState functions. XInputGetKeyState() is used
		/// by the XInputMouse class for managing mouse movement and sensitivity.
		/// </summary>
		const std::map<const std::string, int> xink_buttons =
		{
			{x,VK_PAD_X},
			{y,VK_PAD_Y},
			{a,VK_PAD_A},
			{b,VK_PAD_B},
			{lShoulder,VK_PAD_LSHOULDER},
			{rShoulder,VK_PAD_RSHOULDER},
			{lTrigger, VK_PAD_LTRIGGER},
			{rTrigger, VK_PAD_RTRIGGER},
			{dpad + moreInfo + left, VK_PAD_DPAD_LEFT},
			{dpad + moreInfo + right, VK_PAD_DPAD_RIGHT},
			{dpad + moreInfo + up, VK_PAD_DPAD_UP},
			{dpad + moreInfo + down, VK_PAD_DPAD_DOWN},
			{start, VK_PAD_START},
			{back, VK_PAD_BACK},

			{lThumb, VK_PAD_LTHUMB_PRESS},
			{rThumb, VK_PAD_RTHUMB_PRESS},

			{lThumb + moreInfo + left, VK_PAD_LTHUMB_LEFT},
			{lThumb + moreInfo + right, VK_PAD_LTHUMB_RIGHT},
			{lThumb + moreInfo + up, VK_PAD_LTHUMB_UP},
			{lThumb + moreInfo + down, VK_PAD_LTHUMB_DOWN},
			{lThumb + moreInfo + upleft, VK_PAD_LTHUMB_UPLEFT},
			{lThumb + moreInfo + upright, VK_PAD_LTHUMB_UPRIGHT},
			{lThumb + moreInfo + downleft, VK_PAD_LTHUMB_DOWNLEFT},
			{lThumb + moreInfo + downright, VK_PAD_LTHUMB_DOWNRIGHT},

			{rThumb + moreInfo + left, VK_PAD_RTHUMB_LEFT},
			{rThumb + moreInfo + right, VK_PAD_RTHUMB_RIGHT},
			{rThumb + moreInfo + up, VK_PAD_RTHUMB_UP},
			{rThumb + moreInfo + down, VK_PAD_RTHUMB_DOWN},
			{rThumb + moreInfo + upleft, VK_PAD_RTHUMB_UPLEFT},
			{rThumb + moreInfo + upright, VK_PAD_RTHUMB_UPRIGHT},
			{rThumb + moreInfo + downleft, VK_PAD_RTHUMB_DOWNLEFT},
			{rThumb + moreInfo + downright, VK_PAD_RTHUMB_DOWNRIGHT}
		};
	};

}