/*
Small helper class for simulating input.
*/

#pragma once
#include "stdafx.h"

namespace sds
{
	/// <summary>
	/// Utility class for simulating input via Windows API.
	/// SendInput is used primarily for simulating keyboard and mouse input.
	/// The class is extremely re-usable code.
	/// </summary>
	class SendKey
	{
		INPUT keyInput;
		INPUT mouseInput;
	public:
		/// <summary>
		/// Default Constructor
		/// </summary>
		SendKey()
		{
			memset(&keyInput,0,sizeof(INPUT));
			memset(&mouseInput,0,sizeof(INPUT));
			keyInput.type = INPUT_KEYBOARD;
			mouseInput.type = INPUT_MOUSE;
		}
		/// <summary>
		/// Sends input, if a VK Virtual Keycode of 0 is used, it is assumed to
		/// be mouse input, and keyboard input otherwise.
		/// </summary>
		/// <param name="vk"> is the Virtual Keycode of the keystroke you wish to emulate, use 0 for mouse input</param>
		/// <param name="down"> is a boolean denoting if the keypress event is KEYDOWN or KEYUP</param>
		void Send(int vk, bool down)
		{
			if( down )
				keyInput.ki.dwFlags = KEYEVENTF_SCANCODE;
			else
				keyInput.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
		
			WORD scanCode = GetScanCode(vk);
			keyInput.ki.wScan = scanCode;
			if( scanCode == 0 )
			{
				//Assume mouse.
				switch( vk )
				{
				case VK_LBUTTON:
					if(down)
						mouseInput.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
					else
						mouseInput.mi.dwFlags = MOUSEEVENTF_LEFTUP;
					break;
				case VK_RBUTTON:
					if(down)
						mouseInput.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
					else
						mouseInput.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
					break;
				case VK_MBUTTON:
					if(down)
						mouseInput.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
					else
						mouseInput.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
					break;
				default:
					break;
				}
			}
		
			mouseInput.mi.dwExtraInfo = GetMessageExtraInfo();
			keyInput.ki.dwExtraInfo = GetMessageExtraInfo();
			UINT ret = SendInput(1,(scanCode != 0 ? &keyInput : &mouseInput),sizeof(INPUT));
			assert(ret != 0);
		}
		/// <summary>
		/// Sends a whole string of characters at a time, keydown or keyup.
		/// </summary>
		/// <param name="str"> a string of input to be sent</param>
		/// <param name="down"> denotes a keyup or keydown event</param>
		void Send(std::string str, bool down)
		{
			for(auto it = str.begin(); it != str.end(); ++it)
				Send(*it,down);
		}
	private:
		/// <summary>
		/// Utility function to map a Virtual Keycode to a scancode
		/// </summary>
		/// <param name="vk"> integer virtual keycode</param>
		/// <returns></returns>
		WORD GetScanCode(int vk)
		{
			WORD ret =
				(MapVirtualKeyExA(VkKeyScanA(isalpha(vk) ? tolower(vk) : vk), MAPVK_VK_TO_VSC,GetKeyboardLayout(0)));
			if( ret == 0 )
				ret = static_cast<WORD> (MapVirtualKeyExA(vk, MAPVK_VK_TO_VSC,GetKeyboardLayout(0)));
			
			return ret;
		}
	};
}