#pragma once
#include "stdafx.h"

namespace sds
{
	namespace Utilities
	{
		namespace MapFunctions
		{
			/// <summary>
			/// Verifies that the key value is in the map{T,H} where H is tuple{E,E,F}
			/// </summary>
			/// <param name="keyValue">key value to check</param>
			///	<param name="curMap">map of T,H to check the key</param>
			///	<param name="retVal">will be set to the value the key points to, if found</param>
			/// <returns>true if found, false otherwise</returns>
			template<class T, class H, class E, class F>
			bool IsInMap(const T keyValue, const std::map<T, H>& curMap, H& retVal)
			{
				auto itx = std::find_if(curMap.begin(), curMap.end(), [&keyValue](const std::pair<std::string, std::tuple<E, E, F>>& elem)
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
			///	<param name="curMap">map of T,H to check the key</param>
			/// <returns>true if found, false otherwise</returns>
			template<class T, class H>
			bool IsInMap(const T keyValue, const std::map<T, H>& curMap)
			{
				auto itx = std::find_if(curMap.begin(), curMap.end(), [&keyValue](const std::pair<T, H>& elem)
					{
						return elem.first == keyValue;
					});
				return (itx != curMap.end());
			}
			/// <summary>
			/// Verifies that the key value is in the map{T,H} and sets a reference arg to the value
			///	if found.
			/// </summary>
			/// <param name="keyValue">key value to check</param>
			///	<param name="curMap">map of T,H to check the key</param>
			///	<param name="retVal">will be set to the value the key points to, if found</param>
			/// <returns>true if found, false otherwise</returns>
			template<class T, class H>
			bool IsInMap(const T keyValue, const std::map<T, H>& curMap, H& retVal)
			{
				auto itx = std::find_if(curMap.begin(), curMap.end(), [&keyValue](const std::pair<T, H>& elem)
					{
						return elem.first == keyValue;
					});
				if (itx != curMap.end())
					retVal = itx->second;
				return (itx != curMap.end());
			}
		}
	}
}