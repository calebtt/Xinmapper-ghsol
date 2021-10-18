#pragma once
#include "stdafx.h"
#include "PlayerInfo.h"
#include "ActionDescriptors.h"

namespace sds
{
	static PlayerInfo sdsPlayerOne;
	static ActionDescriptors sdsActionDescriptors;
	static std::string ERR1("Exception, Empty Virtual Function Called.");

	/// <summary>
	/// ActionDetails is a std::string specifically used to transmit state information
	/// about input from the controller to the various classes that will process the information.
	/// </summary>
	typedef std::string ActionDetails;
}