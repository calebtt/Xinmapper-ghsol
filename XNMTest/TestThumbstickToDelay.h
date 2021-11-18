#pragma once
#include "pch.h"
#include "CppUnitTest.h"
#include "..\stdafx.h"
#include "..\Mapper.h"
#include "..\SendKey.h"
#include "..\ActionDescriptors.h"
#include "..\ThumbstickToDelay.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace XNMTest
{
	TEST_CLASS(TestThumbstickToDelay)
	{
		const int Sens = 35;
		const int SensMax = 100;
		const int deadzone = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;

		const short SMax = std::numeric_limits<SHORT>::max();
		const short SMin = std::numeric_limits<SHORT>::min();

		TEST_METHOD(TestGetDelayFromThumbstickValue)
		{
			sds::ThumbstickToDelay delay(Sens, deadzone);
			auto testValues = [&delay](int first, int second, int comparison)
			{
				int result = delay.GetRangedThumbstickValue(first, second);
				Logger::WriteMessage(std::to_string(result).c_str());
				Assert::IsTrue(result == comparison);
			};
			int last = 0;
			auto testGreaterEqualToLast = [&delay](int first, int second, int &last)
			{
				int result = delay.GetRangedThumbstickValue(first, second);
				//Logger::WriteMessage(std::to_string(result).c_str());
				Assert::IsTrue(result >= last, std::to_wstring(result).c_str());
				last = result;
			};
			//test certain values resulting in certain results
			testValues(SMax, deadzone, 100);
			testValues(SMax, 0, 100);
			int fiftyPercent = (SMax / 2) + (deadzone/2);
			testValues(fiftyPercent, deadzone, 50);
			testValues(deadzone+1, deadzone, 1);
			
			//test negative and zero thumbstick values
			testValues(0, deadzone, 1);
			testValues(-1, deadzone, 1);
			testValues(SMin, -deadzone, 100);
			for (int i = 1; i < SMax; i++)
			{
				testGreaterEqualToLast(i, deadzone, last);
			}

		}
	};
}

