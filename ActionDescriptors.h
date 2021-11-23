#pragma once
#include "stdafx.h"

/*
Responsible for tokens used in an ActionDetails
string.
Inevitably tokens must be mapped to values (some of them).
*/



namespace sds
{

	/// <summary>
	/// ActionDescriptors is a big structure full of keywords that are used by other classes to enable
	/// processing of an sds::ActionDetails string into meaningful information for the program.
	/// It has a map&lt;string,int&gt; named "xin_buttons" that is very useful for mapping the string into XINPUT defines
	/// It also has member functions for validating each field of a properly formed token.
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
		const std::string norm = "NORM"; // the string "NORM"
		const std::string toggle = "TOGGLE"; // the string "TOGGLE"
		const std::string rapid = "RAPID"; // the string "RAPID"

		const char moreInfo = ':'; // the char ':'
		const char delimiter = ' ';//spacebar space

		const std::vector<std::string> FirstFieldValidKeywords
		{
			x,y,a,b,lThumb,rThumb,lTrigger,rTrigger,lShoulder,rShoulder,dpad,start,back
		};

		const std::vector<std::string> SecondFieldValidKeywords
		{
			left,down,up,right,none
		};

		const std::vector<std::string> ThirdFieldValidKeywords
		{
			norm,toggle,rapid
		};


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

		/// <summary>
		/// This member function can be used to verify that a string is
		/// a member const keyword included in this struct.
		/// </summary>
		/// <param name="s">std::string s, the token you would test for acceptability in the first field.</param>
		/// <returns>returns true if string s is in the valid first field keywords list</returns>
		constexpr bool IsFirstFieldKeyword(std::string s) const
		{
			std::for_each(s.begin(), s.end(), [](char &c) { c = static_cast<char>(std::toupper(c)); });
			return (std::find(FirstFieldValidKeywords.cbegin(), FirstFieldValidKeywords.cend(), s) != FirstFieldValidKeywords.cend());
		}

		/// <summary>
		/// This member function can be used to verify that a string is
		/// a member const keyword included in this struct.
		/// </summary>
		/// <param name="s">std::string s, the token you would test for acceptability in the second field.</param>
		/// <returns>returns true if string s is in the valid second field keywords list</returns>
		constexpr bool IsSecondFieldKeyword(std::string s) const
		{
			std::for_each(s.begin(), s.end(), [](char &c) { c = static_cast<char>(std::toupper(c)); });
			return (std::find(SecondFieldValidKeywords.cbegin(), SecondFieldValidKeywords.cend(), s) != SecondFieldValidKeywords.cend());
		}

		/// <summary>
		/// This member function can be used to verify that a string is
		/// a member const keyword included in this struct.
		/// </summary>
		/// <param name="s">std::string s, the token you would test for acceptability in the third field.</param>
		/// <returns>returns true if string s is in the valid third field keywords list</returns>
		constexpr bool IsThirdFieldKeyword(std::string s) const
		{
			std::for_each(s.begin(), s.end(), [](char &c) { c = static_cast<char>(std::toupper(c)); });
			return (std::find(ThirdFieldValidKeywords.cbegin(), ThirdFieldValidKeywords.cend(), s) != ThirdFieldValidKeywords.cend());
		}

		/// <summary>
		/// This member function can be used to verify that a string is
		/// a member const keyword included in this struct.
		/// For the fourth field, if it contains one character it returns true.
		/// If it starts with VK and also contains an integer number after, it returns true.
		/// </summary>
		/// <param name="s">std::string s, the token you would test for acceptability in the fourth field.</param>
		/// <returns>returns true if valid field, false otherwise</returns>
		constexpr bool IsFourthFieldKeyword(const std::string s) const
		{
			//single character case, good
			if (s.size() == 1)
				return true;
			

			//check for starting with "VK" but also has a decimal value after
			if (s.size() > vk.size())
			{
				std::string theFirst = s.substr(0, vk.size());
				std::for_each(theFirst.begin(), theFirst.end(), [](char &c)
					{
						if(!std::isupper(c))
							c = static_cast<char>(std::toupper(c));
					});
				//if first two characters are "VK"
				if (theFirst == vk)
				{
					//grab everything after "VK"
					const std::string val = s.substr(vk.size());//everything after "VK"
					int vki = 0;
					//be sure to catch the "out of range exception" if some massive value is attempted
					try
					{
						vki = std::stoi(val); // can throw an exception
						// virtual keycodes fit in a character.
						if (vki > std::numeric_limits<unsigned char>::max() || vki < std::numeric_limits<unsigned char>::min())
							return false;
						else
							return true;
					}
					catch (...)
					{
						return false;
					}

				}
			}

			return false;
		}
	};

}