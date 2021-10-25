/*
Responsible for producing an ActionDetails from
an XINPUT_STATE for consumption by the rest of the code.
*/
#pragma once
#include "stdafx.h"
#include "ButtonStateDown.h"

namespace sds
{
	/// <summary>
	/// Base class for producing an sds::ActionDetails string from an [Input] lib struct for consumption by the rest
	/// of the code. They are space delimited, the tokens are sds::MapInformation tokens.
	/// <example>
	/// This might look like: "X B LTRIGGER RTRIGGER LTHUMB:UP RTHUMB:DOWN"
	/// and another part of the code can use that information to simulate the input mapped to those.
	/// </example>
	/// </summary>
	template <class InputLibType>
	class InputTranslaterBase
	{
	public:
		InputTranslaterBase()
		{
		}
		virtual ~InputTranslaterBase()
		{
		}
		/// <summary>
		/// Produces an ActionDetails string from an [Input] lib struct representing the current state
		/// of the controller, as in what buttons are depressed, what values the thumbsticks are at.
		/// </summary>
		/// <param name="state">state obj retrieved from [Input] lib</param>
		/// <returns>ActionDetails string with the information of which buttons are depressed, 
		/// which thumbsticks and their direction values. Whitespace delimited.
		/// This might look like: "X B LTRIGGER RTRIGGER LTHUMB:UP RTHUMB:DOWN"</returns>
		virtual ActionDetails ProcessState(const InputLibType &state) = 0;
	};

}