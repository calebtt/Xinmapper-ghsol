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
		const int DefaultDeadzone = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;

		const short SMax = std::numeric_limits<SHORT>::max();
		const short SMin = std::numeric_limits<SHORT>::min();

		void PrintUniqueToLoggerAndClear(std::vector<int> &valuesList) const
		{
			auto newEnd = std::unique(valuesList.begin(), valuesList.end());
			std::for_each(valuesList.begin(), newEnd, [](int i)
				{
					std::string message = "Value: " + std::to_string(i);
					Logger::WriteMessage(message.c_str());
				});
			valuesList.clear();
		}
		TEST_METHOD(TestGetRangedThumbstickValue)
		{
			const std::wstring TestName = L"TestGetRangedThumbstickValue()";
			Logger::WriteMessage(std::wstring(L"Begin " + TestName).c_str());

			sds::PlayerInfo pl;
			sds::MouseMap mp = sds::MouseMap::RIGHT_STICK;
			sds::ThumbstickToDelay delay(Sens,pl,mp);

			auto testValues = [&delay](int first, int second, int comparison)
			{
				const int result = delay.GetRangedThumbstickValue(first, second);
				Assert::IsTrue(result == comparison, std::to_wstring(result).c_str());
			};
			int last = 0;
			auto testGreaterEqualToLast = [&delay](int first, int second, int &last)
			{
				const int result = delay.GetRangedThumbstickValue(first, second);
				Assert::IsTrue(result >= last, std::to_wstring(result).c_str());
				last = result;
			};
			//test certain values resulting in certain results
			testValues(SMax, DefaultDeadzone, 100);
			testValues(SMax, 0, 100);
			const int fiftyPercent = (SMax / 2) + (DefaultDeadzone/2);
			testValues(fiftyPercent, DefaultDeadzone, 50);
			testValues(DefaultDeadzone+1, DefaultDeadzone, 1);
			
			//test negative and zero thumbstick values
			testValues(0, DefaultDeadzone, 1);
			testValues(-1, DefaultDeadzone, 1);
			testValues(SMin, -DefaultDeadzone, 100);
			for (int i = 1; i < SMax; i++)
			{
				testGreaterEqualToLast(i, DefaultDeadzone, last);
			}
			//build a list of returned values, then remove duplicates and print the list
			//to the test output
			std::vector<int> valuesList;
			last = 0;
			for (int i = 1; i < SMax; i++)
			{
				valuesList.push_back(delay.GetRangedThumbstickValue(i, DefaultDeadzone));
				testGreaterEqualToLast(i, DefaultDeadzone, last);
			}

			PrintUniqueToLoggerAndClear(valuesList);

			//build a list of returned values from negative thumbstick values
			valuesList.clear();
			for (int i = SMin; i < 0; i++)
			{
				valuesList.push_back(delay.GetRangedThumbstickValue(i, DefaultDeadzone));
			}

			PrintUniqueToLoggerAndClear(valuesList);

			Logger::WriteMessage(std::wstring(L"End " + TestName).c_str());
		}

		//Method to test the overload of GetDelayFromThumbstickValue
		TEST_METHOD(TestGetDelayFromThumbstickValues)
		{
			const sds::PlayerInfo pl;
			const sds::MouseMap mp = sds::MouseMap::RIGHT_STICK;
			sds::ThumbstickToDelay delay(SensMax, pl, mp);
			std::map<int, int> sensMap = delay.GetCopyOfSensitivityMap();
			const int localdz = DefaultDeadzone;
			auto testValues = [&delay, &localdz, &sensMap](int first, int second, size_t comparison)
			{
				//do a computation to get the transformed value expected from GetDelayFromThumbstickValues
				const size_t dualResult = delay.GetDelayFromThumbstickValue(first, second, true);
				Assert::IsTrue(dualResult == comparison);
			};
			//known good test, half magnitude positive both axes
			//should yield a delay magnitude of Sens or sensitivity max,
			//remember the max sensitivity will be truncated per the sensitivity you
			//decide to use.
			testValues(SMax / 2, SMax / 2, sensMap[Sens]);
		}
	};
}

