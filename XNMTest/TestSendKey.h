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
			constexpr int RandomStringCount = 500000;
			constexpr int RandomStringLength = 3;
			constexpr int MinimumRandomStringLength = 1;

			sds::Utilities::SendKey sk;
			// 
			// WARNING:
			// 
			// Some virtual keycodes documented as "undefined" or "reserved" in the Windows documentation
			// actually do return a scancode value.
			// I hope that this is not system dependent.
			BuildVKList vkl;
			//std::vector<int> &&codesList = vkl.GetKeyboardVirtualKeys();
			std::vector<int> &&codesList = vkl.GetSystemVirtualKeys();


			//lambda to test a scan code, works for int, string, char
			auto testScancode = [&sk](auto i)
			{
				std::string outMessage = " Testing Val: ";
				outMessage += i;
				outMessage += " ";
				outMessage += typeid(i).name();
				std::wstring errMsg;
				std::copy(outMessage.begin(), outMessage.end(), std::back_inserter(errMsg));
				//Logger::WriteMessage(outMessage.c_str());
				const bool testCond = sk.GetScanCode(i) > 0;
				Assert::IsTrue(testCond,errMsg.c_str());
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
						const bool testCond = sk.GetScanCode(s) > 0;
						const bool isFoundInKnownGoodList = (std::find(std::cbegin(codesList), std::cend(codesList), i) != std::cend(codesList));
						std::wstring msgg;

						//if it returns true, then assert is must also be in the known good list
						if (isFoundInKnownGoodList && testCond)
						{

						}
						else if((!isFoundInKnownGoodList) && testCond)
						{
							if (std::isprint(i))
							{
								//printable characters are also acceptable input to SendKey::GetScanCode()
								//so if the integer is ALSO translatable to an ASCII keycode, it would still return true.
								msgg = L"Found a (random) true result interpretable as a printable character: ";
								msgg += i;
								msgg += L"\n";
								Logger::WriteMessage(msgg.c_str());
							}
							else
							{
								std::wstring temp;
								std::copy(s.begin(), s.end(), std::back_inserter(temp));
								msgg = L"Found a (random) true result not in the known good list: " + temp;
								msgg += L" translated to int in TestGetVK as: ";
								msgg += std::to_wstring(i);
								//Logger::WriteMessage(msgg.c_str());
								Assert::IsTrue(isFoundInKnownGoodList,msgg.c_str());
							}

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