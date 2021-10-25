#pragma once
#include "stdafx.h"
#include "PlayerInfo.h"
#include "ActionDescriptors.h"

namespace sds
{
	/// <summary>
	/// Used as a global data structure to hold player information, includes
	/// thumbstick and trigger deadzone information.
	/// </summary>
	static PlayerInfo sdsPlayerOne;

	/// <summary>
	/// ActionDescriptors is a struct containing string tokens used to build a string describing how controller
	/// buttons are mapped to Keyboard & Mouse buttons. It also has a std::map xin_buttons mapping some string tokens
	/// to XINPUT lib defines.
	/// </summary>
	static ActionDescriptors sdsActionDescriptors;

	/// <summary>
	/// ActionDetails is a std::string specifically used to transmit state information
	/// about input from the controller to the various classes that will process the information.
	/// Meaning an ActionDetails string will have something like "X B LTRIGGER RTRIGGER LTHUMB:UP RTHUMB:DOWN"
	/// that will be processed into simulated input. The string is generated in XInputTranslater.
	/// </summary>
	using ActionDetails = std::string;

	/// <summary>
	/// Created with input from the user (of the classes), it will likely
	/// be a plain text string describing how to respond to an ActionDetails
	/// 
	/// The string can be tokenized into "words" separated by spaces,
	/// each "word" can be further subdivided into four parts.
	/// 
	/// <list type="bullet">
	/// <listheader><term>ex.</term></listheader>
	/// <item>
	/// LTHUMB:LEFT:NORM:a 
	/// </item>
	/// <item>
	/// LTHUMB:RIGHT:NORM:d
	/// </item>
	/// <item>
	/// LTHUMB:UP:NORM:w
	/// </item>
	/// <item>
	/// LTHUMB:DOWN:NORM:s
	/// </item>
	/// </list>
	/// <br/>
	/// They are of the form: {btn/trigr/thumb}:{moreinfo}:{input sim type}:{value mapped to}
	/// </summary>
	using MapInformation = std::string;

}