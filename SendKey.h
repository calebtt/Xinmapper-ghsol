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
		/// <summary>
		/// Default Constructor
		/// </summary>
		SendKey()
		{
			memset(&keyInput, 0, sizeof(INPUT));
			memset(&mouseClickInput, 0, sizeof(INPUT));
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
			if (down)
				keyInput.ki.dwFlags = KEYEVENTF_SCANCODE;
			else
				keyInput.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;

			WORD scanCode = GetScanCode(vk);
			keyInput.ki.wScan = scanCode;
			if (scanCode == 0)
			{
				//Assume mouse.
				switch (vk)
				{
				case VK_LBUTTON:
					if (down)
						mouseClickInput.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
					else
						mouseClickInput.mi.dwFlags = MOUSEEVENTF_LEFTUP;
					break;
				case VK_RBUTTON:
					if (down)
						mouseClickInput.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
					else
						mouseClickInput.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
					break;
				case VK_MBUTTON:
					if (down)
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
			UINT ret = SendInput(1, (scanCode != 0 ? &keyInput : &mouseClickInput), sizeof(INPUT));
			assert(ret != 0);
		}
		/// <summary>
		/// Sends a whole string of characters at a time, keydown or keyup.
		/// </summary>
		/// <param name="str"> a string of input to be sent</param>
		/// <param name="down"> denotes a keyup or keydown event</param>
		void Send(const std::string str, const bool down)
		{
			for (auto it = str.cbegin(); it != str.cend(); ++it)
				Send(*it, down);
		}

		/// <summary>
		/// Utility function to map a Virtual Keycode to a scancode
		/// </summary>
		/// <param name="vk"> integer virtual keycode</param>
		/// <returns></returns>
		WORD GetScanCode(const int vk)
		{
			if (vk > std::numeric_limits<SHORT>::max() || vk < std::numeric_limits<SHORT>::min())
				return 0;
			WORD ret =
				(MapVirtualKeyExA(VkKeyScanA(isalpha(vk) ? tolower(vk) : vk), MAPVK_VK_TO_VSC, GetKeyboardLayout(0)));
			if (ret == 0)
				ret = static_cast<WORD> (MapVirtualKeyExA(vk, MAPVK_VK_TO_VSC, GetKeyboardLayout(0)));

			return ret;
		}

		/// <summary>
		/// Utility function to map a character to a scancode
		/// If the "char" is a character, it is translated to a VK before returning the Scan Code.
		/// </summary>
		/// <param name="vk">char character such as 'a'</param>
		/// <returns>0 on error, ScanCode otherwise. Retval of 0 indicates no translation available.</returns>
		WORD GetScanCode(const char vk)
		{
			char someCharacter = vk;
			bool isSomeCharacter = static_cast<bool>(std::isprint(vk));
			if (isSomeCharacter)
				someCharacter = static_cast<char>(std::tolower(vk));

			WORD ret = 0;
			if (isSomeCharacter)
			{
				return someCharacter;
				//ret = MapVirtualKeyExA(VkKeyScanA(someCharacter), MAPVK_VK_TO_VSC, GetKeyboardLayout(0));
				//if (ret == 0)
				//{
				//	return MapVirtualKeyExA(someCharacter, MAPVK_VK_TO_VSC, GetKeyboardLayout(0));
				//}
				//else
				//	return ret;
			}
			else
				return MapVirtualKeyExA(VkKeyScanA(someCharacter), MAPVK_VK_TO_VSC, GetKeyboardLayout(0));
		}

		/// <summary>
		/// Utility function to map a Virtual Keycode to a scancode
		/// The virtual keycode is represented as a string of characters.
		/// Ex: "27" for VK_ESCAPE
		/// </summary>
		/// <param name="vk"> integer virtual keycode as a string</param>
		/// <returns>0 on error, ScanCode otherwise. Retval of 0 indicates no translation available.</returns>
		WORD GetScanCode(const std::string vk)
		{
			std::stringstream ss(vk);
			int vki = 0;
			ss >> vki;
			if (!ss)
			{
				//stringstream is reporting an error condition, failure to translate.
				return 0;
			}
			WORD ret =
				(MapVirtualKeyExA(VkKeyScanA(std::isalpha(vki) ? std::tolower(vki) : vki), MAPVK_VK_TO_VSC, GetKeyboardLayout(0)));
			if (ret == 0)
				ret = static_cast<WORD> (MapVirtualKeyExA(vki, MAPVK_VK_TO_VSC, GetKeyboardLayout(0)));

			return ret;
		}
	};
}