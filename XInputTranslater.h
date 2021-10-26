/*
Responsible for producing an ActionDetails from
an XINPUT_STATE for consumption by the rest of the code.
*/
#pragma once
#include "stdafx.h"
#include "ButtonStateDown.h"
#include "InputTranslaterBase.h"

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
	class XInputTranslater : public InputTranslaterBase<XINPUT_STATE>
	{
		//Utility class with functions that test button/thumbstick/trigger for depressed or "down" status
		ButtonStateDown *bsd;
		static const int NumTokens = 10;
		const std::string thumbDirectionList[NumTokens] =
		{
			(sds::sdsActionDescriptors.lThumb + sds::sdsActionDescriptors.moreInfo + sds::sdsActionDescriptors.up),
			(sds::sdsActionDescriptors.lThumb + sds::sdsActionDescriptors.moreInfo + sds::sdsActionDescriptors.down),
			(sds::sdsActionDescriptors.lThumb + sds::sdsActionDescriptors.moreInfo + sds::sdsActionDescriptors.left),
			(sds::sdsActionDescriptors.lThumb + sds::sdsActionDescriptors.moreInfo + sds::sdsActionDescriptors.right),
			(sds::sdsActionDescriptors.rThumb + sds::sdsActionDescriptors.moreInfo + sds::sdsActionDescriptors.up),
			(sds::sdsActionDescriptors.rThumb + sds::sdsActionDescriptors.moreInfo + sds::sdsActionDescriptors.down),
			(sds::sdsActionDescriptors.rThumb + sds::sdsActionDescriptors.moreInfo + sds::sdsActionDescriptors.left),
			(sds::sdsActionDescriptors.rThumb + sds::sdsActionDescriptors.moreInfo + sds::sdsActionDescriptors.right)
		};
	public:
		XInputTranslater()
		{
			bsd = new ButtonStateDown();
		}
		~XInputTranslater()
		{
			delete bsd;
		}
		/// <summary>
		/// Produces an ActionDetails string from an XINPUT_STATE struct representing the current state
		/// of the controller, as in what buttons are depressed, what values the thumbsticks are at.
		/// </summary>
		/// <param name="state">state obj retrieved from XInputGetState()</param>
		/// <returns>ActionDetails string with the information of which buttons are depressed, 
		/// which thumbsticks and their direction values. Whitespace delimited.
		/// This might look like: "X B LTRIGGER RTRIGGER LTHUMB:UP RTHUMB:DOWN"</returns>
		ActionDetails ProcessState(const XINPUT_STATE &state)
		{
			ActionDetails details;

			//Buttons
			for(auto it = sds::sdsActionDescriptors.xin_buttons.cbegin(); it != sds::sdsActionDescriptors.xin_buttons.cend(); ++it)
			{
				if( bsd->ButtonDown(state,it->first ) ) 
				{
					details += it->first + sds::sdsActionDescriptors.delimiter;
				}
			}
			//Triggers
			if( bsd->TriggerDown(state,sds::sdsActionDescriptors.lTrigger) )
			{
				details += sds::sdsActionDescriptors.lTrigger + sds::sdsActionDescriptors.delimiter;
			}
			if( bsd->TriggerDown(state,sds::sdsActionDescriptors.rTrigger) )
			{
				details += sds::sdsActionDescriptors.rTrigger + sds::sdsActionDescriptors.delimiter;
			}
			//Thumbsticks
			for (int i = 0; i < NumTokens; i++)
			{
				if (bsd->ThumbstickDown(state, thumbDirectionList[i]))
				{
					details += thumbDirectionList[i] + sds::sdsActionDescriptors.delimiter;
				}
			}
			return details;
		}
	};

}