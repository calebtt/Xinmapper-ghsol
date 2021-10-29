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
		INPUT mouseClickInput;
		INPUT mouseMoveInput;
	public:
		//enum class MouseEventType
		//{
		//	LEFT_DOWN = MOUSEEVENTF_LEFTDOWN,
		//	LEFT_UP = MOUSEEVENTF_LEFTUP,
		//	RIGHT_DOWN = MOUSEEVENTF_RIGHTDOWN,
		//	RIGHT_UP = MOUSEEVENTF_RIGHTUP,
		//	MIDDLE_DOWN = MOUSEEVENTF_MIDDLEDOWN,
		//	MIDDLE_UP = MOUSEEVENTF_MIDDLEUP,
		//	MOUSE_MOVE = MOUSEEVENTF_MOVE

		//};
		/// <summary>
		/// Default Constructor
		/// </summary>
		SendKey()
		{
			memset(&keyInput,0,sizeof(INPUT));
			memset(&mouseClickInput,0,sizeof(INPUT));
			memset(&mouseMoveInput, 0, sizeof(INPUT));
			keyInput.type = INPUT_KEYBOARD;
			mouseClickInput.type = INPUT_MOUSE;
			mouseMoveInput.type = INPUT_MOUSE;
			mouseMoveInput.mi.dwFlags = MOUSEEVENTF_MOVE;
		}
		/// <summary>
		/// Sends mouse movement specified by X and Y number of pixels to move.
		/// </summary>
		/// <param name="x">number of pixels in X</param>
		/// <param name="y">number of pixels in Y</param>
		void SendMouseMove(const int x, const int y)
		{
			mouseMoveInput.mi.dx = x;
			mouseMoveInput.mi.dy = y;
			mouseMoveInput.mi.dwExtraInfo = GetMessageExtraInfo();

			//Finally, send the input
			SendInput(1, &mouseMoveInput, sizeof(INPUT));
		}
		/// <summary>
		/// Sends input, if a VK Virtual Keycode of 0 is used, it is assumed to
		/// be mouse input, and keyboard input otherwise.
		/// Sends mouse "click" type events, and keyboard events.
		/// </summary>
		/// <param name="vk"> is the Virtual Keycode of the keystroke you wish to emulate, use 0 for mouse input</param>
		/// <param name="down"> is a boolean denoting if the keypress event is KEYDOWN or KEYUP</param>
		void Send(const int vk, const bool down)
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
						mouseClickInput.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
					else
						mouseClickInput.mi.dwFlags = MOUSEEVENTF_LEFTUP;
					break;
				case VK_RBUTTON:
					if(down)
						mouseClickInput.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
					else
						mouseClickInput.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
					break;
				case VK_MBUTTON:
					if(down)
						mouseClickInput.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
					else
						mouseClickInput.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
					break;
				default:
					break;
				}
			}
		
			mouseClickInput.mi.dwExtraInfo = GetMessageExtraInfo();
			keyInput.ki.dwExtraInfo = GetMessageExtraInfo();
			UINT ret = SendInput(1,(scanCode != 0 ? &keyInput : &mouseClickInput),sizeof(INPUT));
			assert(ret != 0);
		}
		/// <summary>
		/// Sends a whole string of characters at a time, keydown or keyup.
		/// </summary>
		/// <param name="str"> a string of input to be sent</param>
		/// <param name="down"> denotes a keyup or keydown event</param>
		void Send(const std::string str, const bool down)
		{
			for(auto it = str.cbegin(); it != str.cend(); ++it)
				Send(*it,down);
		}
	private:
		/// <summary>
		/// Utility function to map a Virtual Keycode to a scancode
		/// </summary>
		/// <param name="vk"> integer virtual keycode</param>
		/// <returns></returns>
		WORD GetScanCode(const int vk)
		{
			WORD ret =
				(MapVirtualKeyExA(VkKeyScanA(isalpha(vk) ? tolower(vk) : vk), MAPVK_VK_TO_VSC,GetKeyboardLayout(0)));
			if( ret == 0 )
				ret = static_cast<WORD> (MapVirtualKeyExA(vk, MAPVK_VK_TO_VSC,GetKeyboardLayout(0)));
			
			return ret;
		}
	};
}