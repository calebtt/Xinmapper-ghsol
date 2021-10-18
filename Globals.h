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
	/// </summary>
	typedef std::string ActionDetails;
}