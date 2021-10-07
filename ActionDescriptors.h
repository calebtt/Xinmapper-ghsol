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
		const std::string x = "X",
			y = "Y",
			a = "A",
			b = "B",
			lThumb = "LTHUMB",
			rThumb = "RTHUMB",
			lTrigger = "LTRIGGER",
			rTrigger = "RTRIGGER",
			lShoulder = "LSHOULDER",
			rShoulder = "RSHOULDER",
			dpad = "DPAD",
			left = "LEFT",
			down = "DOWN",
			up = "UP",
			right = "RIGHT",
			none = "NONE",
			start = "START",
			back = "BACK",
			vk = "VK",
			mappedTo = "=",
			norm = "NORM",
			toggle = "TOGGLE",
			rapid = "RAPID";
		const char moreInfo = ':';
		const char delimiter = ' ';//spacebar space

		//Maps the tokens above to XINPUT library #defines
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