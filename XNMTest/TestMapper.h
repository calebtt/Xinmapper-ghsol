#pragma once
#include "pch.h"
#include "CppUnitTest.h"
#include "..\Mapper.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace XNMTest
{
	TEST_CLASS(TestMapper)
	{
		const int VKMax = 254; // max value a virtual keycode can have.
		const int VKMin = 1; // min value a virtual keycode can have.
	public:
		TEST_METHOD(TestSetMapInfo)
		{
			Logger::WriteMessage("Begin TestSetMapInfo()");

			//valid characters that may be included.
			const std::string InputAlphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-=,./`[];\'"
				+ std::string("!@#$%^&*()_+?{}~");



			Logger::WriteMessage("End TestSetMapInfo()");
		}
	};

}