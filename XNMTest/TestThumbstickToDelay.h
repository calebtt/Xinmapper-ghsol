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
		const int Sens = 100;
		const int SensMax = 100;
		const int DefaultDeadzone = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;

		inline static constexpr const short SMax = std::numeric_limits<SHORT>::max();
		inline static constexpr const short SMin = std::numeric_limits<SHORT>::min();

		TEST_METHOD(TestGetRangedThumbstickValue)
		{
			using namespace TemplatesForTest;
			const std::wstring TestName = L"TestGetRangedThumbstickValue()";
			Logger::WriteMessage(std::wstring(L"Begin " + TestName).c_str());

			sds::PlayerInfo pl;
			sds::MouseMap mp = sds::MouseMap::RIGHT_STICK;
			sds::ThumbstickToDelay delay(Sens,pl,mp);

			auto testValues = [&delay](const int thumbstick, const int deadzone, const int comparison, std::wstring msg = L"Tested ")
			{
				const int result = delay.GetRangedThumbstickValue(thumbstick, deadzone);
				msg += std::to_wstring(thumbstick);
				msg += L" Result: " + std::to_wstring(result);
				Assert::IsTrue(result == comparison, msg.c_str());
			};

			//test certain values resulting in certain results
			testValues(SMax, DefaultDeadzone, 100);
			testValues(SMax, 0, 100);
			const int fiftyPercent = (SMax / 2) + (DefaultDeadzone / 2);
			testValues(fiftyPercent, DefaultDeadzone, 50);
			testValues(DefaultDeadzone + 1, DefaultDeadzone, 1);

			//test negative and zero thumbstick values
			testValues(0, DefaultDeadzone, 1);
			testValues(-1, DefaultDeadzone, 1);
			testValues(SMin, -DefaultDeadzone, 100);

			Logger::WriteMessage(std::wstring(L"End " + TestName).c_str());
		}

		//Method to test the overload of GetDelayFromThumbstickValue
		TEST_METHOD(TestGetDelayFromThumbstickValue)
		{
			using namespace TemplatesForTest;
			const std::wstring TestName = L"TestGetDelayFromThumbstickValue()";
			Logger::WriteMessage(std::wstring(L"Begin " + TestName).c_str());
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
				msg += L" ended with result: " + std::to_wstring(dualResult);
				msg += L" but expected: " + std::to_wstring(comparison);
				msg += L" within: " + std::to_wstring(within);
				const bool resultWithin = IsWithin(dualResult, comparison, within);
				Assert::IsTrue(resultWithin, msg.c_str());
			};
			//Test that thumbstick max value delay returned is XinSettings::MICROSECONDS_MIN +/- 50
			testValues(SMax, 0, true, sds::XinSettings::MICROSECONDS_MIN);
			testValues(SMin, 0, true, sds::XinSettings::MICROSECONDS_MIN);
			testValues(0, SMax, false, sds::XinSettings::MICROSECONDS_MIN);
			testValues(0, SMin, false, sds::XinSettings::MICROSECONDS_MIN);
			//Test that thumbstick 0,0 values delay returned is XinSettings::MICROSECONDS_MAX +/- 300
			testValues(0, 0, true, sds::XinSettings::MICROSECONDS_MAX,300);
			//Compute values for 50% of the thumbstick range beyond the deadzone.
			const int temp = SMax - ((SMax - localdz) / 2);
			const int usTemp = sds::XinSettings::MICROSECONDS_MAX - ((sds::XinSettings::MICROSECONDS_MAX - sds::XinSettings::MICROSECONDS_MIN) / 2);
			testValues(temp, 0, true, usTemp,200);
			Logger::WriteMessage(std::wstring(L"End " + TestName).c_str());
		}
	};
}

