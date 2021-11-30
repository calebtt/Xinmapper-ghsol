#pragma once
#include "pch.h"
#include "CppUnitTest.h"
#include "TemplatesForTest.h"
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

		inline static constexpr const short SMax = std::numeric_limits<SHORT>::max();
		inline static constexpr const short SMin = std::numeric_limits<SHORT>::min();

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
		TEST_METHOD(TestGetDelayFromThumbstickValue)
		{
			using namespace TemplatesForTest;
			//std::map<int, int> sensMap = delay.GetCopyOfSensitivityMap();
			const int localdz = DefaultDeadzone;
			const int localSensMax = SensMax;
			auto testValues = [&localSensMax, &localdz](const int thumbValueX, const int thumbValueY, const bool isX, const size_t comparison, const int within = 50)
			{
				const sds::PlayerInfo pl;
				const sds::MouseMap mp = sds::MouseMap::RIGHT_STICK;
				sds::ThumbstickToDelay delay(localSensMax, pl, mp);

				//do a computation to get the transformed value expected from GetDelayFromThumbstickValues
				const size_t dualResult = delay.GetDelayFromThumbstickValue(thumbValueX, thumbValueY, isX);
				std::wstring msg = L"Tested: X" + std::to_wstring(thumbValueX);
				msg += L" Y:" + std::to_wstring(thumbValueY);
				msg += L" with result: " + std::to_wstring(dualResult);
				const bool resultWithin = IsWithin(dualResult, comparison, within);
				Assert::IsTrue(resultWithin, msg.c_str());
			};
			//Test that thumbstick max value delay returned is XinSettings::MICROSECONDS_MIN +/- 50
			testValues(SMax, 0, true, sds::XinSettings::MICROSECONDS_MIN);
			//Test that thumbstick deadzone+1 value delay returned is XinSettings::MICROSECONDS_MAX +/- 50
			testValues(DefaultDeadzone+1, 0, true, sds::XinSettings::MICROSECONDS_MAX);
			//known good test, half magnitude positive both axes
			//should yield a delay magnitude of Sens or sensitivity max,
			//remember the max sensitivity will be truncated per the sensitivity you
			//decide to use.
			//testValues(SMax / 2, SMax / 2, sensMap[Sens]);
		}
	};
}

