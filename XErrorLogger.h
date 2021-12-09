#pragma once
#include "stdafx.h"

namespace sds
{
	namespace Utilities
	{
		namespace XErrorLogger
		{
			/// <summary>
			/// One function called to log errors, to "cerr" at the moment.
			///	Can be disabled easily or redirected here.
			/// </summary>
			/// <param name="s"></param>
			inline void LogError(std::string_view s)
			{
				std::cerr << s << std::endl;
			}
		}
	}
}

