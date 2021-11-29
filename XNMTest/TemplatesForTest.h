#pragma once
#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace XNMTest
{
	namespace TemplatesForTest
	{
		bool IsWithin(const auto result, const auto testVal, const auto within)
		{
			return ((result > (testVal - within)) && (result < (testVal + within)));
		}
	}
}
