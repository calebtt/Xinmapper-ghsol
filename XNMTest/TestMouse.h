#pragma once
#include "pch.h"
#include "CppUnitTest.h"
#include "..\stdafx.h"
#include "..\Mapper.h"
#include "..\SendKey.h"
#include "..\ActionDescriptors.h"
#include "..\XInputBoostMouse.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace XNMTest
{
	TEST_CLASS(TestMouse)
	{
		sds::XInputBoostMouse mouse;
	public:
		TEST_METHOD(TestSetSensitivity)
		{
			//test some good cases
			Assert::IsTrue(mouse.SetSensitivity(1).size() == 0);
			Assert::IsTrue(mouse.SetSensitivity(100).size() == 0);
			Assert::IsTrue(mouse.SetSensitivity(50).size() == 0);
			//test some edge cases
			Assert::IsTrue(mouse.SetSensitivity(-1).size());
			Assert::IsTrue(mouse.SetSensitivity(0).size());
			Assert::IsTrue(mouse.SetSensitivity(101).size());
			//test some edge cases for integer value extremes
			constexpr const long long IntMax = std::numeric_limits<int>::max();
			constexpr const long long IntMin = std::numeric_limits<int>::min();
			Assert::IsTrue(mouse.SetSensitivity(IntMax).size());
			Assert::IsTrue(mouse.SetSensitivity(IntMin).size());

		}
	};
}
