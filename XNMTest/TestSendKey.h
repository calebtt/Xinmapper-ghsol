#pragma once
#include "pch.h"
#include "CppUnitTest.h"
#include "BuildVKList.h"
#include "BuildRandomStrings.h"
#include "..\SendKey.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace XNMTest
{
	TEST_CLASS(TestSendKey)
	{
	public:
		TEST_METHOD(TestGetVK)
		{
			Logger::WriteMessage("Begin TestGetVK()");

			//Change these here to increase/decrease the test data size.
			//Some const variables affecting the size of the random	test data. 
			const int RandomStringCount = 500000;
			const int RandomStringLength = 3;
			const int MinimumRandomStringLength = 1;

			sds::SendKey sk;
			BuildVKList vkl;
			std::vector<int> &&codesList = vkl.GetKeyboardVirtualKeys();

			//lambda to test a scan code, works for int, string, char
			auto testScancode = [&sk](auto i)
			{
				std::string outMessage = " Testing Val: ";
				outMessage += i;
				outMessage += " ";

				outMessage += typeid(i).name();
				Logger::WriteMessage(outMessage.c_str());
				bool testCond = sk.GetScanCode(i) > 0;
				Assert::IsTrue(testCond);
			};

			//test valid scan code range
			std::for_each(codesList.cbegin(), codesList.cend(), testScancode);

			//test char version
			const std::string InputAlphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-=,./`[];\'"
				+ std::string("!@#$%^&*()_+?{}~");

			std::for_each(InputAlphabet.cbegin(), InputAlphabet.cend(), testScancode);

			//build list of string from codesList
			std::vector <std::string> stringCodesList;
			auto buildStringList = [&stringCodesList](const int i)
			{
				stringCodesList.push_back(std::to_string(i));
			};
			std::for_each(codesList.cbegin(), codesList.cend(), buildStringList);

			//test all the known good strings
			std::for_each(stringCodesList.cbegin(), stringCodesList.cend(), testScancode);

			//build vector of random string data
			BuildRandomStrings brs;
			std::vector<std::string> &&randomStrings = brs.BuildRandomStringVector(RandomStringCount, RandomStringLength, MinimumRandomStringLength);
			//lambda to test a string and compare it to the list of known good strings
			auto randomStringTest = [&sk, &codesList](const std::string &s)
			{
				try
				{
					std::stringstream sst(s);
					int i = 0;
					sst >> i;
					if (sst)
					{
						bool testCond = sk.GetScanCode(s) > 0;
						bool isFoundInKnownGoodList = (std::find(std::cbegin(codesList), std::cend(codesList), i) != std::cend(codesList));

						//if it returns true, then assert is must also be in the known good list
						if (isFoundInKnownGoodList && testCond)
						{

						}
						else if((!isFoundInKnownGoodList) && testCond)
						{
							std::string msgg = "Found a true result not in the known good list: " + s + "\n";
							Logger::WriteMessage(msgg.c_str());
							Assert::IsTrue(isFoundInKnownGoodList);
						}
					}
				}
				catch (...)
				{
					std::string emsg = "EXCEPTION testing: " + s;
					Logger::WriteMessage(emsg.c_str());
				}
			};
			std::for_each(randomStrings.cbegin(), randomStrings.cend(), randomStringTest);
			Logger::WriteMessage("End TestGetVK()");
		}
	};

}