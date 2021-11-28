#pragma once
#include "stdafx.h"

namespace sds
{
	struct SensitivityMap
	{
	private:
		
	public:
		/// <summary>
		/// Builds a sensitivity map that maps values from sens_min to sens_max to values between
		///	us_delay_min and us_delay_max
		/// </summary>
		///	<exception cref="std::string">throws std::string on exception with detailed information.</exception>
		///	<param name="user_sens">user sensitivity value</param>
		/// <param name="sens_min">minimum sensitivity value</param>
		/// <param name="sens_max">maximum sensitivity value</param>
		/// <param name="us_delay_min">minimum delay in microseconds</param>
		/// <param name="us_delay_max">maximum delay in microseconds</param>
		///	<param name="us_delay_min_max">minimum microsecond delay maximum value, used by the user sensitivity adjustment function</param>
		/// <returns></returns>
		std::map<int, int> BuildSensitivityMap(const int user_sens, 
			const int sens_min, 
			const int sens_max, 
			const int us_delay_min, 
			const int us_delay_max, 
			const int us_delay_min_max) const
		{
			//arg error checking
			const std::string m_except_message = "Exception in SensitivityMap::BuildSensitivityMap(): ";
			if (sens_min >= sens_max || us_delay_min >= us_delay_max || user_sens < sens_min || user_sens > sens_max)
				throw std::string(m_except_message + "user sensitivity, or sensitivity range or delay range out of range.");

			//getting new minimum using minimum maximum
			const int adjustedMinimum = SensitivityToMinimum(user_sens, sens_min, sens_max, us_delay_min, us_delay_min_max);

			//building map 
			const int step = (us_delay_max - adjustedMinimum) / (sens_max - sens_min);
			if (step <= 0)
				throw std::string(m_except_message + "computed step value <= 0");

			std::map<int, int> sens_map;
			for (int i = sens_min, j = us_delay_max; i <= sens_max; i++, j-=step)
			{
				if (j < adjustedMinimum)
				{
					sens_map[i] = adjustedMinimum;
				}
				else
				{
					sens_map[i] = j;
				}
			}
			//
			return sens_map;
		}

		/// <summary>
		/// Returns the user sensitivity adjusted minimum microsecond delay based
		///	on the arguments. This is used to alter the minimum microsecond delay of the sensitivity map.
		/// </summary>
		///	<exception cref="std::string"> throws a string containing an error message on invalid arguments given.</exception>
		/// <param name="user_sens">a user sensitivity value</param>
		/// <param name="sens_min">minimum possible sensitivity value</param>
		/// <param name="sens_max">maximum possible sensitivity value</param>
		/// <param name="us_delay_min">minimum delay in microseconds for the range of minimum delay values</param>
		/// <param name="us_delay_max">maximum delay in microseconds for the range of minimum delay values</param>
		/// <returns></returns>
		int SensitivityToMinimum(const int user_sens, 
			const int sens_min, 
			const int sens_max, 
			const int us_delay_min, 
			const int us_delay_max) const
		{
			//arg error checking
			const std::string m_except_message = "Exception in SensitivityMap::SensitivityToMinimum(): ";
			if (sens_min >= sens_max || us_delay_min >= us_delay_max || user_sens < sens_min || user_sens > sens_max)
				throw std::string(m_except_message + "user sensitivity, or sensitivity range or delay range out of range.");

			const double sensitivityRange = (double)sens_max - (double)sens_min;
			const double step = ((double)us_delay_max - (double)us_delay_min) / sensitivityRange;
			
			std::vector<double> delayVec;
			for (auto i = sens_min, j = 0; i <= sens_max; i++, j++)
				delayVec.push_back((double)(us_delay_min) + ((double)j * step));
			//std::reverse(delayVec.begin(), delayVec.end());
			//adapt user_sens and sensitivityRange to vector indexes
			const int elementIndex = sens_max - user_sens;
			return static_cast<int>(delayVec.at(elementIndex));
		}

		/// <summary>
		/// Verifies that the key value is in the map.
		/// </summary>
		/// <param name="txVal">key value to check</param>
		///	<param name="curMap">map of int,int to check the key</param>
		///	<param name="retVal">will be set to the value the key points to, if found</param>
		/// <returns>true if found, false otherwise</returns>
		bool IsInMap(const int txVal, const std::map<int, int> &curMap, int &retVal) const
		{
			auto itx = std::find_if(curMap.begin(), curMap.end(), [&txVal](const std::pair<int, int> &elem)
				{
					return elem.first == txVal;
				});
			if (itx != curMap.end())
				retVal = itx->second;
			return (itx != curMap.end());
		}


	};
}

