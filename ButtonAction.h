#pragma once
#include "stdafx.h"

namespace sds
{
	/// <summary>
	/// Utility class that aids in determining if an XINPUT_STATE has the token
	/// information buttons depressed.
	/// </summary>
	class ButtonAction
	{
	public:
		/// <summary>
		/// Utility function that aids in determining if the button is pressed in the XINPUT_STATE
		/// </summary>
		/// <param name="state"> an XINPUT_STATE with current input from the controller</param>
		/// <param name="token"> a string specifying the button info for comparison</param>
		/// <returns>true if the button is depressed, false otherwise</returns>
		bool Down(const XINPUT_STATE &state, std::string token)
		{
			//std::const_iterator used for access
			for(auto it = sds::sdsActionDescriptors.xin_buttons.cbegin(); it != sds::sdsActionDescriptors.xin_buttons.cend(); ++it)
			{
				if( it->first == token )
				{
					if( state.Gamepad.wButtons & it->second )
						return true;
				}
			}
			return false;
		}
	};
}