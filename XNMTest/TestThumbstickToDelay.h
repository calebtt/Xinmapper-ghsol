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
			sds::PlayerInfo pl;
			sds::MouseMap mp = sds::MouseMap::RIGHT_STICK;
			sds::ThumbstickToDelay delay(Sens,pl,mp);

			auto testValues = [&delay](int first, int second, int comparison)
			{
				const int result = delay.GetRangedThumbstickValue(first, second);
				Logger::WriteMessage(std::to_string(result).c_str());
				Assert::IsTrue(result == comparison);
			};
			int last = 0;
			auto testGreaterEqualToLast = [&delay](int first, int second, int &last)
			{
				const int result = delay.GetRangedThumbstickValue(first, second);
				//Logger::WriteMessage(std::to_string(result).c_str());
				Assert::IsTrue(result >= last, std::to_wstring(result).c_str());
				last = result;
			};
			//test certain values resulting in certain results
			testValues(SMax, deadzone, 100);
			testValues(SMax, 0, 100);
			const int fiftyPercent = (SMax / 2) + (deadzone/2);
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
			//build a list of returned values, then remove duplicates and print the list
			//to the test output
			std::vector<int> valuesList;
			for (int i = 1; i < SMax; i++)
			{
				valuesList.push_back(delay.GetRangedThumbstickValue(i, deadzone));
				//testGreaterEqualToLast(i, deadzone, last);
			}
			auto newEnd = std::unique(valuesList.begin(), valuesList.end());
			std::for_each(valuesList.begin(), newEnd, [](int i)
				{
					Logger::WriteMessage(std::to_string(i).c_str());
				});
			//build a list of returned values from negative thumbstick values
			valuesList.clear();
			for (int i = SMin; i < 0; i++)
			{
				valuesList.push_back(delay.GetRangedThumbstickValue(i, deadzone));
			}
			newEnd = std::unique(valuesList.begin(), valuesList.end());
			std::for_each(valuesList.begin(), newEnd, [](int i)
				{
					Logger::WriteMessage(std::to_string(i).c_str());
				});
		}

		TEST_METHOD(TestGetDelayFromThumbstickValues)
		{
			sds::PlayerInfo pl;
			sds::MouseMap mp = sds::MouseMap::RIGHT_STICK;
			sds::ThumbstickToDelay delay(Sens, pl, mp);
			std::map<int, int> sensMap = delay.GetCopyOfSensitivityMap();
			const int localdz = deadzone;
			auto testValues = [&delay, &localdz, &sensMap](int first, int second, int comparison)
			{
				//gets value representing the thumbstick value in the range of sensitivity_min to sensitivity_max
				int xPercent = delay.GetRangedThumbstickValue(first, localdz);
				int yPercent = delay.GetRangedThumbstickValue(second, localdz);

				//gets delay value for each thumbstick value after the above translation
				const int currentX = sensMap[xPercent];
				const int currentY = sensMap[yPercent];

				//do a computation to get the transformed value expected from GetDelayFromThumbstickValues

				int result = delay.GetDelayFromThumbstickValue(first, second);
				Logger::WriteMessage(std::to_string(result).c_str());
				Assert::IsTrue(result == comparison);
			};
			//known good test, half magnitude positive both axes
			//should yield a delay magnitude of Sens or sensitivity max,
			//remember the max sensitivity will be truncated per the sensitivity you
			//decide to use.
			testValues(SMax / 2, SMax / 2, sensMap[Sens]);

			//int last = 0;
			//auto testGreaterEqualToLast = [&delay](int first, int second, int &last)
			//{
			//	int result = delay.GetRangedThumbstickValue(first, second);
			//	//Logger::WriteMessage(std::to_string(result).c_str());
			//	Assert::IsTrue(result >= last, std::to_wstring(result).c_str());
			//	last = result;
			//};
			////test certain values resulting in certain results
			//testValues(SMax, deadzone, 100);
			//testValues(SMax, 0, 100);
			//int fiftyPercent = (SMax / 2) + (deadzone / 2);
			//testValues(fiftyPercent, deadzone, 50);
			//testValues(deadzone + 1, deadzone, 1);

			////test negative and zero thumbstick values
			//testValues(0, deadzone, 1);
			//testValues(-1, deadzone, 1);
			//testValues(SMin, -deadzone, 100);
			//for (int i = 1; i < SMax; i++)
			//{
			//	testGreaterEqualToLast(i, deadzone, last);
			//}
			////build a list of returned values, then remove duplicates and print the list
			////to the test output
			//std::vector<int> valuesList;
			//for (int i = 1; i < SMax; i++)
			//{
			//	valuesList.push_back(delay.GetRangedThumbstickValue(i, deadzone));
			//	//testGreaterEqualToLast(i, deadzone, last);
			//}
			//auto newEnd = std::unique(valuesList.begin(), valuesList.end());
			//std::for_each(valuesList.begin(), newEnd, [](int i)
			//	{
			//		Logger::WriteMessage(std::to_string(i).c_str());
			//	});
			////build a list of returned values from negative thumbstick values
			//valuesList.clear();
			//for (int i = SMin; i < 0; i++)
			//{
			//	valuesList.push_back(delay.GetRangedThumbstickValue(i, deadzone));
			//}
			//newEnd = std::unique(valuesList.begin(), valuesList.end());
			//std::for_each(valuesList.begin(), newEnd, [](int i)
			//	{
			//		Logger::WriteMessage(std::to_string(i).c_str());
			//	});
		}
	};
}

