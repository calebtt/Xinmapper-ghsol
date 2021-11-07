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
				Logger::WriteMessage((ert + ertt).c_str());
				Assert::IsTrue(ertt.size() == 0);
			};
			auto testMapFunctionFalse = [&mp](const std::string &s)
			{
				std::string ert = "Testmap [input]: " + s + " [expect result error message]: ";
				std::string ertt = mp.SetMapInfo(s);
				Logger::WriteMessage((ert + ertt).c_str());
				Assert::IsFalse(ertt.size() == 0);
			};

			//Test known good string case
			testMapFunctionTrue(mapInfo);

			//test some edge cases and malformed input
			testMapFunctionFalse("LTHUMB:UP:NORM:a DPAD:DWN:NORM:x DPAD:UP:NORM:vk1");
			testMapFunctionFalse("LTHUMB::NORM:a DPAD:DWN:NORM:x DPAD:UP:NORM:vk0");
			Logger::WriteMessage("End TestSetMapInfo()");
		}
	};

}