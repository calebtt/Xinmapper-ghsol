#include "pch.h"
#include "CppUnitTest.h"
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <limits>
#include "..\stdafx.h"
#include "..\ButtonAction.h"
//#include "..\Xinmapper-gh\ActionDescriptors.h"
//#include "..\Xinmapper-gh\Globals.h"

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
		std::mt19937 gen;

	public:
		/// <summary>
		/// Constructor, initializes some useful stuff.
		/// </summary>
		XNMTest()
		{
			//seed random generator
			std::random_device rd;
			gen.seed(rd());

			//Add all std::string members of the ActionDetails struct global static instance
			testTokens.push_back(sds::sdsActionDescriptors.a);
			testTokens.push_back(sds::sdsActionDescriptors.b);
			testTokens.push_back(sds::sdsActionDescriptors.back);
			testTokens.push_back(sds::sdsActionDescriptors.down);
			testTokens.push_back(sds::sdsActionDescriptors.dpad);
			testTokens.push_back(sds::sdsActionDescriptors.left);
			testTokens.push_back(sds::sdsActionDescriptors.lShoulder);
			testTokens.push_back(sds::sdsActionDescriptors.lThumb);
			testTokens.push_back(sds::sdsActionDescriptors.lTrigger);
			testTokens.push_back(sds::sdsActionDescriptors.mappedTo);
			testTokens.push_back(sds::sdsActionDescriptors.none);
			testTokens.push_back(sds::sdsActionDescriptors.norm);
			testTokens.push_back(sds::sdsActionDescriptors.rapid);
			testTokens.push_back(sds::sdsActionDescriptors.right);
			testTokens.push_back(sds::sdsActionDescriptors.rShoulder);
			testTokens.push_back(sds::sdsActionDescriptors.rThumb);
			testTokens.push_back(sds::sdsActionDescriptors.rTrigger);
			testTokens.push_back(sds::sdsActionDescriptors.start);
			testTokens.push_back(sds::sdsActionDescriptors.toggle);
			testTokens.push_back(sds::sdsActionDescriptors.up);
			testTokens.push_back(sds::sdsActionDescriptors.vk);
			testTokens.push_back(sds::sdsActionDescriptors.x);
			testTokens.push_back(sds::sdsActionDescriptors.y);
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

				st.dwPacketNumber = static_cast<DWORD>(gen());
				st.Gamepad.bLeftTrigger = static_cast<BYTE>(gen());
				st.Gamepad.bRightTrigger = static_cast<BYTE>(gen());
				st.Gamepad.sThumbLX = static_cast<SHORT>(gen());
				st.Gamepad.sThumbLY = static_cast<SHORT>(gen());
				st.Gamepad.sThumbRX = static_cast<SHORT>(gen());
				st.Gamepad.sThumbRY = static_cast<SHORT>(gen());
				st.Gamepad.wButtons = static_cast<WORD>(gen());
				ret.push_back(st);
			}
			
			return ret;
		}
		
		/// <summary>
		/// Returns a vector of std::string with randomized content.
		/// count is the number of entries in the returned vector, length is the max length of the strings.
		/// </summary>
		/// <returns> a vector of std::string with randomized content. Empty vector on error. </returns>
		std::vector<std::string> BuildRandomStringVector(const int count, const int length, const int minLength = 3)
		{
			//arg error checking, returns empty vector as per description
			if (minLength >= length || (length <= 0) || (count <=0) || (minLength <=0))
			{
				return std::vector<std::string>();
			}

			//Test all kinds of random character possibilities.
			std::uniform_int_distribution<int> distCharPossibility
			(std::numeric_limits<char>::min(), 
				std::numeric_limits<char>::max());

			std::uniform_int_distribution<int> distLengthPossibility(minLength, length);

			std::random_device rd;
			std::mt19937 stringGenerator(rd());
			std::vector<std::string> ret;

			//the distribution uses the generator engine to get the value
			for (int i = 0; i < count; i++)
			{
				const int tLength = distLengthPossibility(stringGenerator);
				std::string currentBuiltString = "";
				for (int j = 0; j < tLength; j++)
				{
					char currentBuiltChar = distCharPossibility(stringGenerator);
					currentBuiltString += currentBuiltChar;
				}
				ret.push_back(currentBuiltString);
			}

			return ret;
		}

		/// <summary>
		/// Test that the "Down()" member function returns the expected values.
		/// </summary>
		TEST_METHOD(TestButtonActionReturns)
		{
			using namespace std;
			Logger::WriteMessage("Begin TestButtonActionReturns()");

			
			//Change these here to increase/decrease the test data size.
			//Some const variables affecting the size of the random	test data. 
			const int RandomStringCount = 1000000;
			const int RandomStringLength = 8;
			const int RandomStateCount = 1000;

			
			//Xinput lib struct
			XINPUT_STATE testState;
			//zero it
			memset(&testState, 0, sizeof(XINPUT_STATE));
			//subject of our test, ButtonAction
			sds::ButtonAction ba;

			//Test every valid string token in vector<string> testTokens against a zeroed XINPUT_STATE
			//(Test the equivalence class with a zeroed XINPUT_STATE)
			auto tf = [&testState, &ba](string& st) { Assert::IsFalse(ba.Down(testState, st)); };
			for_each(testTokens.begin(), testTokens.end(), tf);


			//Test 1000 randomized XINPUT_STATE objects with a sample token
			//assert that the result ONLY is true with the correct token and state.
			//(Test boundary values against the tokens)
			vector<XINPUT_STATE> tempVector = BuildRandomTestStates(RandomStateCount);
			const map<const string, int> &r = testMap;

			//Lambda helper function to Assert test "testState" and a token
			auto tsv = [&ba, &r, &testState](const string& st)
			{
				if (ba.Down(testState, st))
				{
					//assert that the string is in the map!
					Assert::IsTrue(r.count(st));
					//bitwise AND test if the correct bits are set, compared to the map
					Assert::IsTrue((testState.Gamepad.wButtons & r.at(st)));
				}
			};

			//Warning: Potentially lengthy run-time! O(N^2)
			//iterate vector of randomized XINPUT_STATE values
			for (auto it = tempVector.begin(); it != tempVector.end(); ++it)
			{
				testState = *it;
				//nested loop to test each string token in the map against the current XINPUT_STATE
				for_each(testTokens.begin(), testTokens.end(), tsv);
			}

			//Random strings (tokens) test
			//generate [RandomStringCount] strings up to [RandomStringLength] characters in length
			//(Test boundary values for the string tokens, 
			// the token is mapped to a value in the map which makes this easier)
			vector<string> randomTestTokens = BuildRandomStringVector(RandomStringCount, RandomStringLength);
			for_each(randomTestTokens.begin(), randomTestTokens.end(), tsv);

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
			std::for_each(testTokens.begin(), testTokens.end(), [](std::string& tr) { Assert::IsTrue(tr.size()>0); });

			//Test 'char' for value > 0
			Assert::IsTrue(sds::sdsActionDescriptors.delimiter);
			Assert::IsTrue(sds::sdsActionDescriptors.moreInfo);

			//Test std::map<string,int> for size > 0
			Assert::IsTrue(sds::sdsActionDescriptors.xin_buttons.size() > 0);
			Logger::WriteMessage("End TestActionDescriptorsInit()");
		}
	};
}
