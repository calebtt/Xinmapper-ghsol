#pragma once
#include "pch.h"
#include "CppUnitTest.h"
#include "..\stdafx.h"
#include "..\Mapper.h"
#include "..\SendKey.h"
#include "..\ActionDescriptors.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace XNMTest
{
	TEST_CLASS(TestMapper)
	{
	public:
		TEST_METHOD(TestSetMapInfo)
		{
			Logger::WriteMessage("Begin TestSetMapInfo()");

			//valid characters that may be included.
			const std::string InputAlphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-=,./`[];\'"
				+ std::string("!@#$%^&*()_+?{}~");

			
			sds::MapInformation mapInfo;
			sds::Mapper mp;
			mapInfo = "LTHUMB:LEFT:NORM:a LTHUMB:RIGHT:NORM:d LTHUMB:UP:NORM:w LTHUMB:DOWN:NORM:s X:NONE:NORM:r A:NONE:NORM:VK32 Y:NONE:NORM:VK164 B:NONE:NORM:VK160";
			mapInfo += " LSHOULDER:NONE:NORM:v RSHOULDER:NONE:NORM:Q LTHUMB:NONE:NORM:c RTHUMB:NONE:NORM:e START:NONE:NORM:VK27 BACK:NONE:NORM:VK8 LTRIGGER:NONE:NORM:VK2";
			mapInfo += " RTRIGGER:NONE:NORM:VK1 DPAD:LEFT:NORM:c DPAD:DOWN:NORM:x DPAD:UP:NORM:f DPAD:RIGHT:RAPID:VK1";


			auto testMapFunctionTrue = [&mp](const std::string &s)
			{
				std::string ert = "Testmap [input]: " + s + " [expect result no error message]: ";
				std::string ertt = mp.SetMapInfo(s);
				std::string composite = ert + ertt;
				std::wstring errMsg;
				std::copy(composite.begin(), composite.end(), std::back_inserter(errMsg));
				//Logger::WriteMessage((ert + ertt).c_str());
				Assert::IsTrue(ertt.empty(), errMsg.c_str());
			};
			auto testMapFunctionFalse = [&mp](const std::string &s)
			{
				std::string ert = "Testmap [input]: " + s + " [expect result error message]: ";
				std::string ertt = mp.SetMapInfo(s);
				std::string composite = ert + ertt;
				std::wstring errMsg;
				std::copy(composite.begin(), composite.end(), std::back_inserter(errMsg));
				//Logger::WriteMessage((ert + ertt).c_str());
				Assert::IsFalse(ertt.empty(),errMsg.c_str());
			};

			//Test known good string case
			testMapFunctionTrue(mapInfo);

			//test some edge cases and malformed input
			testMapFunctionFalse("LTHUMB:UP:NORM:a DPAD:DWN:NORM:x DPAD:UP:NORM:vk1");
			testMapFunctionFalse("LTHUMB::NORM:a DPAD:DWN:NORM:x DPAD:UP:NORM:vk0");
			testMapFunctionFalse("LTHUMB::NORM:a DPAD:DWN:NORM:x DPAD:UP:NORM:vk");
			testMapFunctionFalse("LTHUMB:NORM:a DPAD:DWN:NORM:x UP:NORM:vk");
			testMapFunctionFalse(" ");
			testMapFunctionFalse("   \r\n");
			testMapFunctionFalse("");
			testMapFunctionFalse("lThumb:left:nORM:a dPAD:DoWN:nORM:x DPAd:uP:NoRM:vK3333"); // 3333 is far too large to be a virtual keycode
			testMapFunctionFalse("lThumb:left:nORM:a dPAD:DoWN:nORM:x DPAd:uP:NoRM:vK33333333333333333333"); // 3333... might overflow an int
			//test some good cases with lower case
			testMapFunctionTrue("lthumb:left:NORM:a DPAD:DoWN:NORM:x DPAD:UP:NORM:vk33");
			

			constexpr long long curType = static_cast<long long>(std::numeric_limits<int>::max()) + 1;
			std::string testStr = "lthumb:left:norm:a dpad:down:norm:x dpad:up:norm:vk" + std::to_string(curType);
			testMapFunctionFalse(testStr);

			//test each character from the inputalphabet
			for (auto it = InputAlphabet.cbegin(); it != InputAlphabet.cend(); ++it)
			{
				std::string mess = "\nTesting char: ";
				mess += *it;
				Logger::WriteMessage(mess.c_str());
				std::string currentBuilt = "lthumb:up:norm:a dpad:down:norm:x dpad:up:norm:";
				currentBuilt += *it;
				testMapFunctionTrue(currentBuilt);
			}

			Logger::WriteMessage("End TestSetMapInfo()");
		}
	};

}