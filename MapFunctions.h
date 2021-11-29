#pragma once
#include "stdafx.h"

namespace sds
{
	namespace MapFunctions
	{
		/// <summary>
		/// Verifies that the key value is in the map.
		/// </summary>
		/// <param name="keyValue">key value to check</param>
		///	<param name="curMap">map of int,int to check the key</param>
		///	<param name="retVal">will be set to the value the key points to, if found</param>
		/// <returns>true if found, false otherwise</returns>
		template<class T, class H>
		static bool IsInMap(const T keyValue, const std::map<T, H> &curMap, H &retVal)
		{
			auto itx = std::find_if(curMap.begin(), curMap.end(), [&keyValue](const std::pair<T, H> &elem)
				{
					return elem.first == keyValue;
				});
			if (itx != curMap.end())
				retVal = itx->second;
			return (itx != curMap.end());
		}

		/// <summary>
		/// Verifies that the key value is in the map.
		/// </summary>
		/// <param name="keyValue">key value to check</param>
		///	<param name="curMap">map of int,int to check the key</param>
		/// <returns>true if found, false otherwise</returns>
		template<class T, class H>
		static bool IsInMap(const T keyValue, const std::map<T, H> &curMap)
		{
			auto itx = std::find_if(curMap.begin(), curMap.end(), [&keyValue](const std::pair<T, H> &elem)
				{
					return elem.first == keyValue;
				});
			return (itx != curMap.end());
		}
	}
}