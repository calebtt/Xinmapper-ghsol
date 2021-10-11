/*
Responsible for producing an ActionDetails from
an XINPUT_STATE for consumption by the rest of the code.
*/
#pragma once
#include "stdafx.h"
#include "ButtonAction.h"
#include "TriggerAction.h"
#include "ThumbstickAction.h"

namespace sds
{
	/// <summary>
	/// Produces an sds::ActionDetails string from an XINPUT_STATE for consumption by the rest
	/// of the code. They are delimited, the tokens are in the form of the sds::MapInformation tokens.
	/// <example>
	/// &lt;btn/trigr/thumb&gt;:&lt;more info&gt;:&lt;input sim type&gt;:&lt;value mapped to&gt;
	/// </example>
	/// </summary>
	class XInputTranslater
	{
		ButtonAction *btn;
		TriggerAction *trg;
		ThumbstickAction *thmb;
		//PlayerInfo *player; // not used
	public:
		XInputTranslater()
		{
			btn = new ButtonAction();
			trg = new TriggerAction();
			thmb = new ThumbstickAction();
		}
		~XInputTranslater()
		{
			delete btn;
			delete trg;
			delete thmb;
		}
		/// <summary>
		/// Produces an ActionDetails string from an XINPUT_STATE struct representing the current state
		/// of the controller, as in what buttons are depressed, what values the thumbsticks are at.
		/// </summary>
		/// <param name="state"></param>
		/// <returns>ActionDetails string with the information of which buttons are depressed, 
		/// which thumbsticks and their direction values</returns>
		ActionDetails ProcessState(const XINPUT_STATE &state)
		{
			ActionDetails details;
			//Buttons
			for(auto it = sds::sdsActionDescriptors.xin_buttons.cbegin(); it != sds::sdsActionDescriptors.xin_buttons.cend(); ++it)
			{
				if( btn->Down(state,it->first ) ) 
				{
					details += it->first + sds::sdsActionDescriptors.delimiter;
				}
			}
			//Triggers
			if( trg->Down(state,sds::sdsActionDescriptors.lTrigger) )
			{
				details += sds::sdsActionDescriptors.lTrigger + sds::sdsActionDescriptors.delimiter;
			}
			if( trg->Down(state,sds::sdsActionDescriptors.rTrigger) )
			{
				details += sds::sdsActionDescriptors.rTrigger + sds::sdsActionDescriptors.delimiter;
			}
			//Thumbsticks
			std::string thumb = sds::sdsActionDescriptors.lThumb + sds::sdsActionDescriptors.moreInfo;
			//lThumb
			if( thmb->Down(state,thumb + sds::sdsActionDescriptors.up) )
			{
				details += thumb + sds::sdsActionDescriptors.up + sds::sdsActionDescriptors.delimiter;
			}
			if( thmb->Down(state,thumb + sds::sdsActionDescriptors.down) )
			{
				details += thumb + sds::sdsActionDescriptors.down + sds::sdsActionDescriptors.delimiter;
			}
			if( thmb->Down(state, thumb + sds::sdsActionDescriptors.left) )
			{
				details += thumb + sds::sdsActionDescriptors.left + sds::sdsActionDescriptors.delimiter;
			}
			if( thmb->Down(state, thumb + sds::sdsActionDescriptors.right) )
			{
				details += thumb + sds::sdsActionDescriptors.right + sds::sdsActionDescriptors.delimiter;
			}

			//rThumb
			thumb = sds::sdsActionDescriptors.rThumb + sds::sdsActionDescriptors.moreInfo;
			if( thmb->Down(state,thumb + sds::sdsActionDescriptors.up) )
			{
				details += thumb + sds::sdsActionDescriptors.up + sds::sdsActionDescriptors.delimiter;
			}
			if( thmb->Down(state,thumb + sds::sdsActionDescriptors.down) )
			{
				details += thumb + sds::sdsActionDescriptors.down + sds::sdsActionDescriptors.delimiter;
			}
			if( thmb->Down(state, thumb + sds::sdsActionDescriptors.left) )
			{
				details += thumb + sds::sdsActionDescriptors.left + sds::sdsActionDescriptors.delimiter;
			}
			if( thmb->Down(state, thumb + sds::sdsActionDescriptors.right) )
			{
				details += thumb + sds::sdsActionDescriptors.right + sds::sdsActionDescriptors.delimiter;
			}
			return details;
		}
	};

}