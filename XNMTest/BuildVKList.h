#pragma once
#include "pch.h"
#include "CppUnitTest.h"
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <vector>

/// <summary>
/// A class like this could save someone a lot of typing.
/// ..Someone..
/// </summary>
struct BuildVKList
{
	

	/// <summary>
	/// Stupidly long constructor that builds a list of virtual keycodes available in Winuser.h
	/// that should have scancode translations available.
	/// </summary>
	BuildVKList()
	{
	}

	/// <summary>
	/// Builds and returns a list of valid keyboard virtual key codes as a vector of int.
	/// </summary>
	/// <returns>a vector of integers representing valid keyboard virtual key codes.</returns>
	std::vector<int> GetKeyboardVirtualKeys()
	{
		std::vector<int> vkList; // a list of valid key codes from Winuser.h
		vkList.push_back(VK_CANCEL);
		// Keycode 0x07 is undefined.

		vkList.push_back(VK_BACK);
		vkList.push_back(VK_TAB);

		// Keycodes hex: 0x0A-0x0B are reserved

		//vkList.push_back(VK_CLEAR);
		vkList.push_back(VK_RETURN);

		// Keycodes hex: 0x0E-0x0F are undefined.
		vkList.push_back(VK_SHIFT);
		vkList.push_back(VK_CONTROL);
		vkList.push_back(VK_MENU);
		//vkList.push_back(VK_PAUSE);
		vkList.push_back(VK_CAPITAL);
		vkList.push_back(VK_ESCAPE);

		for (int i = VK_SPACE; i < VK_DOWN + 1; i++)
		{
			vkList.push_back(i);
		}

		for (int i = VK_SNAPSHOT; i < 0x39 + 1; i++)
		{
			vkList.push_back(i);
		}

		// Keycodes hex: 0x3A-0x40 are undefined.

		for (int i = 0x41; i < VK_APPS + 1; i++)
		{
			vkList.push_back(i);
		}

		// Keycode hex: 0x5E is reserved.

		for (int i = VK_SLEEP; i < VK_ADD + 1; i++)
		{
			vkList.push_back(i);
		}

		for (int i = VK_SUBTRACT; i < VK_F12 + 1; i++)
		{
			vkList.push_back(i);
		}

		// Keycodes hex: 0x88-0x8F are unassigned.

		vkList.push_back(VK_NUMLOCK);
		vkList.push_back(VK_SCROLL);

		// Keycodes 0x92-0x96 are OEM specific.
		// Keycodes 0x97-0x9F are unassigned.

		for (int i = VK_LSHIFT; i < VK_LAUNCH_APP2 + 1; i++)
		{
			vkList.push_back(i);
		}

		// Keycodes hex: 0xB8-0xB9 are reserved

		for (int i = VK_OEM_1; i < VK_OEM_3 + 1; i++)
		{
			vkList.push_back(i);
		}

		// Keycodes hex: 0xC1-0xD7 are reserved
		// Keycodes hex: 0xD8-0xDA are unassigned

		//for (int i = VK_OEM_4; i < VK_OEM_8 + 1; i++)
		//{
		//	vkList.push_back(i);
		//}

		// Keycode hex: 0xE0 is reserved.
		// Keycode hex: 0xE1 is OEM specific.

		vkList.push_back(VK_OEM_102);

		// Keycodes 0xE3-0xE4 are OEM specific.
		//vkList.push_back(VK_PROCESSKEY);

		// Keycode 0xE6 is OEM specific.

		//vkList.push_back(VK_PACKET);

		// Keycode 0xE8 is unassigned.
		// Keycodes 0xE9-0xF5 are OEM specific.

		//for (int i = VK_ATTN; i < VK_ZOOM + 1; i++)
		//{
		//	vkList.push_back(i);
		//}

		// Keycode 0xFC is reserved but has a name "VK_NONAME"

		//vkList.push_back(VK_PA1);
		//vkList.push_back(VK_OEM_CLEAR);
		return vkList;
	}
};

