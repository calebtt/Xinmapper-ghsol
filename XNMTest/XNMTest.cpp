#include "pch.h"
#include "CppUnitTest.h"
#include "TestSendKey.h"
#include "TestMapper.h"
#include "BuildRandomStrings.h"
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <limits>


#include "..\ButtonStateDown.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace XNMTest
{
	TEST_CLASS(XNMTest)
	{
		//List of tokens in an "ActionDetails"
		std::vector<std::string> testTokens;
		//Internal copy of the action descriptors map, mapping string tokens to int value representation of XINPUT defines
		const std::map<const std::string, int> testMap = sds::sdsActionDescriptors.xin_buttons;
		//mt19937 is a standard mersenne_twister_engine
		std::mt19937 mersenneEngine;
		BuildRandomStrings rsb;

	public:
		/// <summary>
		/// Constructor, initializes some useful stuff like testTokens.
		/// </summary>
		XNMTest()
		{
			//seed random generator
			std::random_device rd;
			mersenneEngine.seed(rd());

			std::vector<std::string> initVector;
			auto fillVector = [&initVector](const std::vector<std::string> &currentVec)
			{
				auto addTokens = [&initVector](const std::string &currentString)
				{
					initVector.push_back(currentString);
				};
				std::for_each(currentVec.cbegin(), currentVec.cend(), addTokens);
			};

			fillVector(sds::sdsActionDescriptors.FirstFieldValidKeywords);
			fillVector(sds::sdsActionDescriptors.SecondFieldValidKeywords);
			fillVector(sds::sdsActionDescriptors.ThirdFieldValidKeywords);
			initVector.push_back(sds::sdsActionDescriptors.vk);

			testTokens = initVector;
		}
		/// <summary>
		/// Returns a vector of XINPUT_STATE structs with randomized content.
		/// count is the number of entries in the returned vector.
		/// </summary>
		/// <returns> a vector of XINPUT_STATE with randomized content. Empty vector on error.</returns>
		std::vector<XINPUT_STATE> BuildRandomTestStates(const int count)
		{
			if (count <= 0)
				return std::vector<XINPUT_STATE>();

			std::vector<XINPUT_STATE> ret;
			XINPUT_STATE st;
			for (int i = 0; i < count; i++)
			{
				memset(&st, 0, sizeof(XINPUT_STATE));

				st.dwPacketNumber = static_cast<DWORD>(mersenneEngine());
				st.Gamepad.bLeftTrigger = static_cast<BYTE>(mersenneEngine());
				st.Gamepad.bRightTrigger = static_cast<BYTE>(mersenneEngine());
				st.Gamepad.sThumbLX = static_cast<SHORT>(mersenneEngine());
				st.Gamepad.sThumbLY = static_cast<SHORT>(mersenneEngine());
				st.Gamepad.sThumbRX = static_cast<SHORT>(mersenneEngine());
				st.Gamepad.sThumbRY = static_cast<SHORT>(mersenneEngine());
				st.Gamepad.wButtons = static_cast<WORD>(mersenneEngine());
				ret.push_back(st);
			}

			return ret;
		}

		/// <summary>
		/// Test that the "Down()" member function behaves as expected
		/// by generating input and throwing it at it, along with some edge 
		/// cases and known good cases.
		/// </summary>
		TEST_METHOD(TestButtonActionReturns)
		{
			using namespace std;
			Logger::WriteMessage("Begin TestButtonActionReturns()");


			//Change these here to increase/decrease the test data size.
			//Some const variables affecting the size of the random	test data. 
			const int RandomStringCount = 100000;
			const int RandomStringLength = 8;
			const int RandomStateCount = 10000;


			//Xinput lib struct
			XINPUT_STATE testState;
			//zero it
			memset(&testState, 0, sizeof(XINPUT_STATE));
			//subject of our test, ButtonStateDown
			sds::ButtonStateDown ba;

			//Test every valid string token in vector<string> testTokens against a zeroed XINPUT_STATE
			//(Test the equivalence class with a zeroed XINPUT_STATE)
			auto tf = [&testState, &ba](string &st) { Assert::IsFalse(ba.ButtonDown(testState, st)); };
			for_each(testTokens.begin(), testTokens.end(), tf);


			//Copy map to a local const
			const map<const string, int> &r = testMap;
			//assert the map size is greater than 0
			Assert::IsTrue(r.size());
			
			//Lambda helper function to Assert test "testState" and a token
			auto tsv = [&ba, &r, &testState](const string &st)
			{
				if (ba.ButtonDown(testState, st))
				{
					//assert that the string is in the map!
					Assert::IsTrue(r.count(st));
					//bitwise AND test if the correct bits are set, compared to the map
					Assert::IsTrue((testState.Gamepad.wButtons & r.at(st)));
				}
			};

			try
			{
				//Test [RandomStateCount] randomized XINPUT_STATE objects with a sample token
				//assert that the result ONLY is true with the correct token and state.
				//(Test boundary values against the tokens)
				vector<XINPUT_STATE> &&tempVector = BuildRandomTestStates(RandomStateCount);

				//Warning: Potentially lengthy run-time! O(N^2)
				//iterate vector of randomized XINPUT_STATE values
				for (auto it = tempVector.begin(); it != tempVector.end(); ++it)
				{
					testState = *it;
					//nested loop to test each string token in the map against the current XINPUT_STATE
					for_each(testTokens.begin(), testTokens.end(), tsv);
				}
			}
			catch (std::exception &e)
			{
				Logger::WriteMessage(e.what());
			}

			try
			{
				//Random strings (tokens) test
				//generate [RandomStringCount] strings up to [RandomStringLength] characters in length
				//(Test boundary values for the string tokens, 
				// the token is mapped to a value in the map which makes this easier)
				vector<string> &&randomTestTokens = rsb.BuildRandomStringVector(RandomStringCount, RandomStringLength);
				for_each(randomTestTokens.begin(), randomTestTokens.end(), tsv);
			}
			catch (std::exception &e)
			{
				Logger::WriteMessage(e.what());
			}
			Logger::WriteMessage("End TestButtonActionReturns()");

		}



		/// <summary>
		/// Test for initialization of ActionDescriptors
		/// </summary>
		TEST_METHOD(TestActionDescriptorsInit)
		{
			//Static member sds::sdsActionDescriptors tested here.
			Logger::WriteMessage("Begin TestActionDescriptorsInit()");
			//Test strings for size > 0
			std::for_each(testTokens.begin(), testTokens.end(), [](std::string &tr) { Assert::IsTrue(tr.size() > 0); });

			//Test 'char' for value > 0
			Assert::IsTrue(sds::sdsActionDescriptors.delimiter);
			Assert::IsTrue(sds::sdsActionDescriptors.moreInfo);

			//Test std::map<string,int> for size > 0
			Assert::IsTrue(sds::sdsActionDescriptors.xin_buttons.size() > 0);
			Logger::WriteMessage("End TestActionDescriptorsInit()");
		}
	};
}
