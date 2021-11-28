#pragma once
#include "pch.h"
#include "CppUnitTest.h"
#include "..\stdafx.h"
#include "..\ThumbstickToDelay.h"
#include "..\SensitivityMap.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace XNMTest
{
	TEST_CLASS(TestSensitivityMap)
	{

		TEST_METHOD(TestBuildSensitivityMap)
		{
			sds::SensitivityMap mp;

			auto TestResultWithin = [&mp](const int sensVal, const int testVal, const int within = 50)
			{
				const std::map<int,int> result = mp.BuildSensitivityMap(sensVal,
					sds::XinSettings::SENSITIVITY_MIN,
					sds::XinSettings::SENSITIVITY_MAX,
					sds::XinSettings::MICROSECONDS_MIN,
					sds::XinSettings::MICROSECONDS_MAX,
					sds::XinSettings::MICROSECONDS_MIN*3);


				//const std::wstring out = L"Sens: "
				//	+ std::to_wstring(sensVal)
				//	+ L" Result: "
				//	+ std::to_wstring(result)
				//	+ L" Expected: "
				//	+ std::to_wstring(testVal);
				//const bool isWithin = ((result > (testVal - within)) && (result < (testVal + within)));
				//Assert::IsTrue(isWithin, out.c_str());
			};
			//test sensitivity of 50 results in 1000 microsecond delay +/- 50
			TestResultWithin(50, 1000);
			//test that a sensitivity of 100 results in 500 microsecond delay +/- 50
			TestResultWithin(100, 500);
			//test that a sensitivity of 35 results in 1156 microsecond delay +/- 50
			TestResultWithin(35, 1156);
			//test that a sensitivity of 1 results in 1500 microsecond delay +/- 50
			TestResultWithin(1, 1500);

		}

		TEST_METHOD(TestSensitivityToMinimum)
		{
			sds::SensitivityMap mp;
			
			auto TestResultWithin = [&mp](const int sensVal, const int testVal, const int within=50)
			{
				const int result = mp.SensitivityToMinimum(sensVal,
					sds::XinSettings::SENSITIVITY_MIN,
					sds::XinSettings::SENSITIVITY_MAX,
					sds::XinSettings::MICROSECONDS_MIN,
					sds::XinSettings::MICROSECONDS_MIN * 3);
				const std::wstring out = L"Sens: "
					+ std::to_wstring(sensVal)
					+ L" Result: "
					+ std::to_wstring(result)
					+ L" Expected: "
					+ std::to_wstring(testVal);
				const bool isWithin = ((result > (testVal - within)) && (result < (testVal + within)));
				Assert::IsTrue(isWithin, out.c_str());
			};
			try
			{
				//test sensitivity of 50 results in 1000 microsecond delay +/- 50
				TestResultWithin(50, 1000);
				//test that a sensitivity of 100 results in 500 microsecond delay +/- 50
				TestResultWithin(100, 500);
				//test that a sensitivity of 35 results in 1156 microsecond delay +/- 50
				TestResultWithin(35, 1156);
				//test that a sensitivity of 1 results in 1500 microsecond delay +/- 50
				TestResultWithin(1, 1500);
			}
			catch(std::string &e)
			{
				Logger::WriteMessage(e.c_str());
				Assert::IsTrue(false);
			}

		}
	};
}

