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


struct BuildVKList
{
	BuildVKList()
	{
	}

	/// <summary>
	/// This intends to build the list of virtual keycodes that return a scancode from a call to
	/// MapVirtualKeyExA(...) from the Windows API. It is possible these will be system specific.
	/// </summary>
	/// <returns></returns>
	std::vector<int> GetSystemVirtualKeys()
	{
		std::vector<int> vkList;
		for (UINT i = std::numeric_limits<unsigned char>::min(); i < std::numeric_limits<unsigned char>::max(); i++)
		{
			UINT ret = static_cast<int> (MapVirtualKeyExA(i, MAPVK_VK_TO_VSC, GetKeyboardLayout(0)));
			if(ret != 0)
				vkList.push_back(static_cast<int>(i));
		}
		
		return vkList;
	}
	/// <summary>
	/// Builds and returns a list of valid keyboard virtual key codes as a vector of int.
	/// Some virtual keycodes documented as "undefined" or "reserved" in the Windows documentation
	/// actually do return a scancode value, some are added in here at the end.
	/// I hope that this is not system dependent.
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

		for (int i = VK_SUBTRACT; i < VK_F20 + 1; i++)
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

		for (int i = VK_OEM_1; i < VK_OEM_3 + 1; i++)
		{
			vkList.push_back(i);
		}

		vkList.push_back(VK_OEM_102);

		//undefined but still returns a scancode...
		vkList.push_back(0x3B);
		vkList.push_back(0x3D);
		//reserved but still returns a scancode...
		vkList.push_back(0xC);
		vkList.push_back(0xC1);
		vkList.push_back(0xDD);
		
		//OEM but still returns a scancode...
		vkList.push_back(0xEB);
		vkList.push_back(0xEE);
		vkList.push_back(0xED);
		vkList.push_back(0xF1);
		vkList.push_back(0xF3);
		vkList.push_back(0xF5);
		vkList.push_back(VK_F21);
		vkList.push_back(VK_F22);
		vkList.push_back(VK_F23);
		vkList.push_back(VK_F24);
		vkList.push_back(VK_EREOF);
		vkList.push_back(VK_ZOOM);
		vkList.push_back(VK_OEM_4);
		vkList.push_back(VK_OEM_5);
		vkList.push_back(VK_OEM_7);

		return vkList;
	}
};

