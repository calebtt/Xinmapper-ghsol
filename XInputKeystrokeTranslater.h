/*
Responsible for producing an ActionDetails from
an XINPUT_STATE for consumption by the rest of the code.
*/
#pragma once
#include "stdafx.h"
#include "ButtonStateDown.h"
#include "InputTranslaterBase.h"
#include "SendKey.h"

namespace sds
{
	/// <summary>
	/// Produces an sds::ActionDetails string from an XINPUT_STATE for consumption by the rest
	/// of the code. They are space delimited, the tokens are sds::MapInformation tokens.
	/// <example>
	/// This might look like: "X B LTRIGGER RTRIGGER LTHUMB:UP RTHUMB:DOWN"
	/// and another part of the code can use that information to simulate the input mapped to those.
	/// </example>
	/// </summary>
	class XInputKeystrokeTranslater : public InputTranslaterBase<XINPUT_KEYSTROKE>
	{
		//Utility class with functions that test button/thumbstick/trigger for depressed or "down" status
		ButtonStateDown *bsd;
		SendKey *sk;
	public:
		XInputKeystrokeTranslater()
		{
			bsd = new ButtonStateDown();
			sk = new SendKey();
		}
		~XInputKeystrokeTranslater()
		{
			delete bsd;
			delete sk;
		}
		/// <summary>
		/// Produces an ActionDetails string from an XINPUT_KEYSTROKE struct representing the current state
		/// of the controller, as in what buttons are depressed.
		/// </summary>
		/// <param name="state">state obj retrieved from XInputGetKeystroke()</param>
		/// <returns>ActionDetails string with the information of which buttons are depressed, 
		/// which thumbsticks and their direction values. Whitespace delimited.
		/// This might look like: "X B LTRIGGER RTRIGGER LTHUMB:UP RTHUMB:DOWN"</returns>
		ActionDetails ProcessState(const XINPUT_KEYSTROKE &state)
		{
			ActionDetails details;
			std::string vkPart = sds::sdsActionDescriptors.vk + sds::sdsActionDescriptors.mappedTo; // "VK="
			int ivk = 0;
			//Buttons
			for (auto it = sds::sdsActionDescriptors.xin_buttons.cbegin(); it != sds::sdsActionDescriptors.xin_buttons.cend(); ++it)
			{
				if (bsd->ButtonDown(state, it->first))
				{
					details += it->first + sds::sdsActionDescriptors.delimiter;
				}
			}
			//Xbox360 controller keyboard virtual keys
			if ((ivk=sk->GetScanCode(state.VirtualKey)))
			{
				details += vkPart + std::to_string(state.VirtualKey);
			}
			return details;
		}
	};

}