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
		INPUT m_keyInput = {};
		INPUT m_mouseClickInput = {};
		INPUT m_mouseMoveInput = {};
	public:
		/// <summary>
		/// Default Constructor
		/// </summary>
		SendKey()
		{
			m_keyInput.type = INPUT_KEYBOARD;
			m_mouseClickInput.type = INPUT_MOUSE;
			m_mouseMoveInput.type = INPUT_MOUSE;
			m_mouseMoveInput.mi.dwFlags = MOUSEEVENTF_MOVE;
		}
		SendKey(const SendKey& other) = delete;
		SendKey(SendKey&& other) = delete;
		SendKey& operator=(const SendKey& other) = delete;
		SendKey& operator=(SendKey&& other) = delete;
		~SendKey() = default;
		/// <summary>
		/// Sends mouse movement specified by X and Y number of pixels to move.
		/// </summary>
		/// <param name="x">number of pixels in X</param>
		/// <param name="y">number of pixels in Y</param>
		void SendMouseMove(const int x, const int y)
		{
			m_mouseMoveInput.mi.dx = static_cast<LONG>(x);
			m_mouseMoveInput.mi.dy = static_cast<LONG>(y);
			m_mouseMoveInput.mi.dwExtraInfo = GetMessageExtraInfo();

			//Finally, send the input
			SendInput(1, &m_mouseMoveInput, sizeof(INPUT));
		}
		/// <summary>
		/// Sends mouse movement specified by vector of tuple(int,int) X and Y number of pixels to move.
		/// </summary>
		void SendMouseMove(const std::vector<std::tuple<int,int>> &tup) const
		{
			std::vector<INPUT> inputVec;
			std::for_each(tup.cbegin(), tup.cend(), [&inputVec](const std::tuple<int, int> &t)
				{
					INPUT i = { }; // zeroed struct
					i.type = INPUT_MOUSE;
					i.mi.dwFlags = MOUSEEVENTF_MOVE;
					i.mi.dx = static_cast<LONG>(std::get<0>(t));
					i.mi.dy = static_cast<LONG>(std::get<1>(t));
					i.mi.dwExtraInfo = GetMessageExtraInfo();
					inputVec.push_back(i);
				});

			//Finally, send the input
			SendInput(static_cast<UINT>(inputVec.size()), inputVec.data(), sizeof(INPUT));
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
			const WORD scanCode = GetScanCode(vk);
			//std::cerr << scanCode << std::endl;
			if (scanCode == 0)
			{
				//do scancode version
				switch (vk)
				{
				case VK_LBUTTON:
					if (down)
						m_mouseClickInput.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
					else
						m_mouseClickInput.mi.dwFlags = MOUSEEVENTF_LEFTUP;
					break;
				case VK_RBUTTON:
					if (down)
						m_mouseClickInput.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
					else
						m_mouseClickInput.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
					break;
				case VK_MBUTTON:
					if (down)
						m_mouseClickInput.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
					else
						m_mouseClickInput.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
					break;
				case VK_XBUTTON1:
				case VK_XBUTTON2:
					if (down)
						m_mouseClickInput.mi.dwFlags = MOUSEEVENTF_XDOWN;
					else
						m_mouseClickInput.mi.dwFlags = MOUSEEVENTF_XUP;
				default:
					break;
				}
				m_mouseClickInput.mi.dwExtraInfo = GetMessageExtraInfo();
				SendInput(1, &m_mouseClickInput, sizeof(INPUT));
				return;
			}
			//Else do keyboard version
			else
			{
				m_keyInput.ki.dwFlags = (down ? 0 : KEYEVENTF_KEYUP);
				m_keyInput.ki.wVk = static_cast<WORD>(vk);
				m_keyInput.ki.dwExtraInfo = GetMessageExtraInfo();
				SendInput(1, &m_keyInput, sizeof(INPUT));
				return;
			}
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
		WORD GetScanCode(const int vk) const
		{
			if (vk > std::numeric_limits<unsigned char>::max() || vk < std::numeric_limits<unsigned char>::min())
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
		WORD GetScanCode(const char vk) const
		{
			const char someCharacter = vk;
			bool isSomeCharacter = static_cast<bool>(std::isprint(vk));
			if (isSomeCharacter)
			{
				return static_cast<char>(std::tolower(vk));
			}
			else
			{
				return MapVirtualKeyExA(VkKeyScanA(someCharacter), MAPVK_VK_TO_VSC, GetKeyboardLayout(0));
			}
		}
		/// <summary>
		/// Utility function to map a Virtual Keycode to a scancode
		/// The virtual keycode is represented as a string of characters.
		/// Ex: "27" for VK_ESCAPE
		/// </summary>
		/// <param name="vk"> integer virtual keycode as a string</param>
		/// <returns>0 on error, ScanCode otherwise. Retval of 0 indicates no translation available.</returns>
		WORD GetScanCode(const std::string vk) const
		{
			std::stringstream ss(vk);
			int vki = 0;
			ss >> vki;
			if (!ss || !ss.eof())
			{
				//stringstream is reporting an error condition, failure to translate.
				return 0;
			}
			return GetScanCode(vki);
		}
	};
}