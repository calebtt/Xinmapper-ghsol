#pragma once
#include "stdafx.h"

namespace sds
{
	struct SensitivityMap
	{
	private:
		const std::string m_exceptMinimum = "Exception in SensitivityMap::SensitivityToMinimum(): ";
		const std::string m_exceptBuildMap = "Exception in SensitivityMap::BuildSensitivityMap(): ";
	public:
		/// <summary>
		/// Builds a sensitivity map that maps values from sens_min to sens_max to values between
		/// us_delay_min and us_delay_max
		/// </summary>
		/// <exception cref="std::string">throws std::string on exception with detailed information.</exception>
		/// <param name="user_sens">user sensitivity value</param>
		/// <param name="sens_min">minimum sensitivity value</param>
		/// <param name="sens_max">maximum sensitivity value</param>
		/// <param name="us_delay_min">minimum delay in microseconds</param>
		/// <param name="us_delay_max">maximum delay in microseconds</param>
		/// <param name="us_delay_min_max">minimum microsecond delay maximum value, used by the user sensitivity adjustment function</param>
		/// <returns>map int,int mapping sensitivity values to microsecond delay values</returns>
		[[nodiscard]] std::map<int, int> BuildSensitivityMap(const int user_sens, 
			const int sens_min, 
			const int sens_max, 
			const int us_delay_min, 
			const int us_delay_max, 
			const int us_delay_min_max) const
		{
			//arg error checking
			if (sens_min >= sens_max || us_delay_min >= us_delay_max || user_sens < sens_min || user_sens > sens_max)
				Utilities::XErrorLogger::LogError(m_exceptBuildMap + "user sensitivity, or sensitivity range or delay range out of range.");

			//getting new minimum using minimum maximum
			const int adjustedMinimum = SensitivityToMinimum(user_sens, sens_min, sens_max, us_delay_min, us_delay_min_max);

			//building map 
			const int step = (us_delay_max - adjustedMinimum) / (sens_max - sens_min);
			if (step <= 0)
				Utilities::XErrorLogger::LogError(m_exceptBuildMap + "computed step value <=0");

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
			return sens_map;
		}

		/// <summary>
		/// Returns the user sensitivity adjusted minimum microsecond delay based
		/// on the arguments. This is used to alter the minimum microsecond delay of the sensitivity map,
		/// when a sensitivity value is used.
		/// </summary>
		/// <exception cref="std::string"> throws a string containing an error message on invalid arguments given.</exception>
		/// <param name="user_sens">a user sensitivity value</param>
		/// <param name="sens_min">minimum possible sensitivity value</param>
		/// <param name="sens_max">maximum possible sensitivity value</param>
		/// <param name="us_delay_min">minimum delay in microseconds for the range of minimum delay values</param>
		/// <param name="us_delay_max">maximum delay in microseconds for the range of minimum delay values</param>
		/// <returns>sensitivity adjusted minimum microsecond delay </returns>
		int SensitivityToMinimum(const int user_sens, 
			const int sens_min, 
			const int sens_max, 
			const int us_delay_min, 
			const int us_delay_max) const
		{
			//arg error checking
			
			if (sens_min >= sens_max || us_delay_min >= us_delay_max || user_sens < sens_min || user_sens > sens_max)
			{
				Utilities::XErrorLogger::LogError(m_exceptMinimum + "user sensitivity, or sensitivity range or delay range out of range.");
				return 1;
			}

			const double sensitivityRange = (double)sens_max - (double)sens_min;
			const double step = ((double)us_delay_max - (double)us_delay_min) / sensitivityRange;
			
			std::vector<double> delayVec;
			for (auto i = sens_min, j = 0; i <= sens_max; i++, j++)
				delayVec.push_back((double)(us_delay_min) + ((double)j * step));

			//adapt user_sens and sensitivityRange to vector indexes
			const int elementIndex = sens_max - user_sens;
			return static_cast<int>(delayVec.at(elementIndex));
		}
	};
}

