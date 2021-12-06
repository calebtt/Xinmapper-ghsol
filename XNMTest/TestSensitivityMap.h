#pragma once
#include "pch.h"
#include "CppUnitTest.h"
#include "..\stdafx.h"
#include "..\ThumbstickToDelay.h"
#include "..\SensitivityMap.h"
#include "TemplatesForTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace XNMTest
{
	TEST_CLASS(TestSensitivityMap)
	{

		TEST_METHOD(TestBuildSensitivityMap)
		{
			using namespace TemplatesForTest;
			const std::wstring TestFuncName = L"TestBuildSensitivityMap()";
			Logger::WriteMessage(std::wstring(L"Begin "+ TestFuncName).c_str());
			
			sds::SensitivityMap mp;
			auto GetResultMap = [&mp](const int sensVal)
			{
				std::map<int, int> result = mp.BuildSensitivityMap(sensVal,
					sds::XinSettings::SENSITIVITY_MIN,
					sds::XinSettings::SENSITIVITY_MAX,
					sds::XinSettings::MICROSECONDS_MIN,
					sds::XinSettings::MICROSECONDS_MAX,
					sds::XinSettings::MICROSECONDS_MIN * 3);
				return result;
			};
			const std::map<int, int> result = GetResultMap(100);
			bool testCond = IsWithin(result.crbegin()->second, sds::XinSettings::MICROSECONDS_MIN, 100);
			Assert::IsTrue(testCond, L"TestBuildSensitivityMap() : 100");
			testCond = IsWithin(result.cbegin()->second, sds::XinSettings::MICROSECONDS_MAX, 50);
			Assert::IsTrue(testCond, L"TestBuildSensitivityMap() : 1");
			Logger::WriteMessage(std::wstring(L"End " + TestFuncName).c_str());
		}

		TEST_METHOD(TestSensitivityToMinimum)
		{
			using namespace TemplatesForTest;
			const std::wstring TestFuncName = L"TestSensitivityToMinimum()";
			Logger::WriteMessage(std::wstring(L"Begin " + TestFuncName).c_str());
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
				const bool isWithin = IsWithin(result,testVal,within);
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
			Logger::WriteMessage(std::wstring(L"End " + TestFuncName).c_str());
		}
	};
}

