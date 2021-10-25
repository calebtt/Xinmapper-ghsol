/*
Responsible for tokens used in an ActionDetails
string.
Inevitably tokens must be mapped to values (some of them).
*/

#pragma once
#include "stdafx.h"

namespace sds
{
	/// <summary>
	/// ActionDescriptors is a big structure full of keywords that are used by other classes to enable
	/// processing of an sds::ActionDetails string into meaningful information for the program.
	/// It has a map&lt;string,int&gt; named "xin_buttons" that is very useful for mapping the string into XINPUT #defines
	/// 
	/// </summary>
	struct ActionDescriptors
	{
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
		const std::string down = "DOWN"; // the string "DOWN"
		const std::string up = "UP"; // the string "UP"
		const std::string right = "RIGHT"; // the string "RIGHT"
		const std::string none = "NONE"; // the string "NONE"
		const std::string start = "START"; // the string "START"
		const std::string back = "BACK"; // the string "BACK"
		const std::string vk = "VK"; // the string "VK"
		const std::string mappedTo = "="; // the string "="
		const std::string norm = "NORM"; // the string "NORM"
		const std::string toggle = "TOGGLE"; // the string "TOGGLE"
		const std::string rapid = "RAPID"; // the string "RAPID"

		const char moreInfo = ':'; // the char ':'
		const char delimiter = ' ';//spacebar space

		//Maps the tokens above to XINPUT library #defines
		//Because the XINPUT lib doesn't send a "down" signal 
		//to be tested against with a define bitmask, it
		//isn't mapped to an xinput lib define bitmask here.
		//instead lTrigger, rTrigger are tested against the current value
		//in BYTE bLeftTrigger and bRightTrigger in the XINPUT_GAMEPAD struct
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
	};

}