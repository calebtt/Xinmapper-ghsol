#pragma once
#include "pch.h"
#include <string>
#include <random>
#include <algorithm>
#include <limits>

struct BuildRandomStrings
{
	BuildRandomStrings()
	{
	}
	/// <summary>
	/// Returns a vector of std::string with randomized content.
	/// count is the number of entries in the returned vector, length is the max length of the strings.
	/// </summary>
	/// <returns> a vector of std::string with randomized content. Empty vector on error. </returns>
	auto BuildRandomStringVector(const int count, const int length, const int minLength = 3)
	{
		//arg error checking, returns empty vector as per description
		if (minLength >= length || (length <= 0) || (count <= 0) || (minLength <= 0))
		{
			return std::vector<std::string>();
		}

		//Test all kinds of random character possibilities.
		std::uniform_int_distribution<int> distCharPossibility
		(std::numeric_limits<char>::min(),
			std::numeric_limits<char>::max());

		std::uniform_int_distribution<int> distLengthPossibility(minLength, length);

		std::random_device rd;
		std::mt19937 stringGenerator(rd()); // seed mersenne engine
		std::vector<std::string> ret;


		//lambda used with std::generate to fill the string with chars of randomized content.
		auto getRandomChar = [&distCharPossibility, &stringGenerator]()
		{
			return static_cast<char>(distCharPossibility(stringGenerator));
		};

		//the distribution uses the generator engine to get the value
		for (int i = 0; i < count; i++)
		{
			int tLength = distLengthPossibility(stringGenerator);
			std::string currentBuiltString(tLength, ' ');
			std::generate(currentBuiltString.begin(), currentBuiltString.end(), getRandomChar);
			ret.push_back(currentBuiltString);
		}

		return ret;
	}
};

