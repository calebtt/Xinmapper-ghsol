#pragma once
#include "pch.h"
#include "CppUnitTest.h"
#include "..\SendKey.h"
#include "BuildVKList.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace XNMTest
{
	TEST_CLASS(TestSendKey)
	{
		const int VKMax = 254; // max value a virtual keycode can have.
		const int VKMin = 1; // min value a virtual keycode can have.
	public:
		TEST_METHOD(TestGetVK)
		{
			Logger::WriteMessage("Begin TestGetVK()");
			sds::SendKey sk;
			BuildVKList vkl;
			std::vector<int> &&codesList = vkl.GetKeyboardVirtualKeys();

			auto testScancode = [&sk](auto i)
			{
				std::string s;
				std::string outMessage = "Testing Val: " + std::to_string(i) + " ";
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

			Logger::WriteMessage("End TestGetVK()");
		}
	};

}