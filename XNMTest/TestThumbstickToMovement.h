#pragma once
#include "pch.h"
#include "CppUnitTest.h"
#include "..\stdafx.h"
#include "..\Mapper.h"
#include "..\SendKey.h"
#include "..\ActionDescriptors.h"
#include "..\ThumbstickToMovement.h"
#include <locale>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace XNMTest
{
	TEST_CLASS(TestThumbstickToMovement)
	{
		const int DEADZONE = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
		const int IMax = std::numeric_limits<int>::max();
		const int IMin = std::numeric_limits<int>::min();
		//XINPUT_GAMEPAD thumbstick values are "SHORT"
		const short SMax = std::numeric_limits<SHORT>::max();
		const short SMin = std::numeric_limits<SHORT>::min();
		const int SENS_VAL = 55;
		//for adequate coverage over the range of say, 0 to IMax with a decent runtime
		//we will increment/decrement in steps
		const int StepValue = 9;
		const int PixelsToMoveLow = 5; //5
		const int PixelsToMoveHigh = 55; //55
		

	public:
		TEST_METHOD(TestDoesRequireMove)
		{
			Logger::WriteMessage("Begin TestDoesRequireMove()");

			sds::ThumbstickToMovement mover(SENS_VAL, DEADZONE);

			//Test zero values
			Assert::IsFalse(mover.DoesRequireMove(0, 0));
			//Test positive false values
			for (int i = 0; i < DEADZONE; i++)
			{
				Assert::IsFalse(mover.DoesRequireMove(i, i));
				Assert::IsFalse(mover.DoesRequireMove(0, i));
				Assert::IsFalse(mover.DoesRequireMove(i, 0));
			}
			//Test negative false values
			for (int i = 0; i > -DEADZONE; i--)
			{
				Assert::IsFalse(mover.DoesRequireMove(i, i));
				Assert::IsFalse(mover.DoesRequireMove(0, i));
				Assert::IsFalse(mover.DoesRequireMove(i, 0));
			}
			//Test boundary true values, O(n^2)
			std::vector<int> boundaryArray = { IMax, IMin, 0 };
			for (size_t i = 0; i < boundaryArray.size(); i++)
			{
				for (size_t j = 0; j < boundaryArray.size(); j++)
				{
					int first = boundaryArray.at(i);
					int second = boundaryArray.at(j);

					if (first == 0 && second == 0)
						continue;
					std::string msg = "Testing: ";
					msg += std::to_string(first);
					msg += " And " + std::to_string(second);
					Logger::WriteMessage(msg.c_str());
					Assert::IsTrue(mover.DoesRequireMove(first, second));
				}
			}

			//Test positive true values
			for (int i = DEADZONE + 1; i < IMax-StepValue; i+=StepValue)
			{
				Assert::IsTrue(mover.DoesRequireMove(i, i));
				Assert::IsTrue(mover.DoesRequireMove(0, i));
				Assert::IsTrue(mover.DoesRequireMove(i, 0));
			}
			//Test negative true values
			for (int i = -DEADZONE - 1; i > IMin+StepValue; i-=StepValue)
			{
				Assert::IsTrue(mover.DoesRequireMove(i, i));
				Assert::IsTrue(mover.DoesRequireMove(0, i));
				Assert::IsTrue(mover.DoesRequireMove(i, 0));
			}

			Logger::WriteMessage("End TestDoesRequireMove()");
		}

		TEST_METHOD(TestNormalizeRange)
		{
			Logger::WriteMessage("Begin TestNormalizeRange()");
			sds::ThumbstickToMovement mover(SENS_VAL, DEADZONE);
			//test some true results
			Assert::IsTrue(mover.NormalizeRange(0, IMax, SENS_VAL, DEADZONE + 1));
			Assert::IsTrue(mover.NormalizeRange(0, IMax, SENS_VAL, IMax));
			Assert::IsTrue(mover.NormalizeRange(0, IMax, SENS_VAL, -(IMin+1)));

			Assert::IsTrue(mover.NormalizeRange(0, SMax, SENS_VAL, DEADZONE + 1));
			Assert::IsTrue(mover.NormalizeRange(0, SMax, SENS_VAL, SMax));
			Assert::IsTrue(mover.NormalizeRange(0, SMax, SENS_VAL, -(SMin + 1)));

			//test some false results
			Assert::IsFalse(mover.NormalizeRange(IMax, 0, SENS_VAL, DEADZONE + 1));
			Assert::IsFalse(mover.NormalizeRange(IMax, 0, SENS_VAL, IMax));
			Assert::IsFalse(mover.NormalizeRange(IMax, 0, SENS_VAL, -(IMin + 1)));

			Assert::IsFalse(mover.NormalizeRange(SMax, 0, SENS_VAL, DEADZONE + 1));
			Assert::IsFalse(mover.NormalizeRange(SMax, 0, SENS_VAL, SMax));
			Assert::IsFalse(mover.NormalizeRange(SMax, 0, SENS_VAL, -(SMin + 1)));

			Logger::WriteMessage("End TestNormalizeRange()");
		}

		TEST_METHOD(TestGetPackagedInput)
		{
			Logger::WriteMessage("Begin TestGetPackagedInput()");
			sds::ThumbstickToMovement mover(SENS_VAL, DEADZONE);
			
			//vector to hold the results
			std::vector<std::tuple<int, int>> inputBuf;
			
			auto testInputBuf = [&inputBuf](bool useGreater, bool useEqual, std::wstring testNumber)
			{
				auto getMessage = [](std::tuple<int, int> t)
				{
					std::wstring mes(std::to_wstring(std::get<0>(t)) + L" and " + std::to_wstring(std::get<1>(t)));
					return mes;
				};
				std::wstring message;

				if (useGreater)
				{
					if (useEqual)
					{
						message = testNumber + std::wstring(L" :Begin testing greater than equal zero. ");
						std::for_each(inputBuf.begin(), inputBuf.end(), [&message,&getMessage](std::tuple<int, int> &t)
							{
								message += getMessage(t);
								Assert::IsTrue((std::get<0>(t) >= 0) && (std::get<1>(t) >= 0), message.c_str());
							});
					}
					else
					{
						message = testNumber + std::wstring(L" :Begin testing greater than zero. ");
						std::for_each(inputBuf.begin(), inputBuf.end(), [&message,&getMessage](std::tuple<int, int> &t)
							{
								message += getMessage(t);
								Assert::IsTrue((std::get<0>(t) > 0) && (std::get<1>(t) > 0), message.c_str());
							});
					}
				}
				else
				{
					if (useEqual)
					{
						message = testNumber + std::wstring(L" :Begin testing less than equal zero. ");
						std::for_each(inputBuf.begin(), inputBuf.end(), [&message,&getMessage](std::tuple<int, int> &t)
							{
								message += getMessage(t);
								Assert::IsTrue((std::get<0>(t) <= 0) && (std::get<1>(t) <= 0), message.c_str());
							});
					}
					else
					{
						message = testNumber + std::wstring(L" :Begin testing less than zero. ");
						std::for_each(inputBuf.begin(), inputBuf.end(), [&message,&getMessage](std::tuple<int, int> &t)
							{
								message += getMessage(t);
								Assert::IsTrue((std::get<0>(t) < 0) && (std::get<1>(t) < 0), message.c_str());
							});
					}
				}

			};

			auto assertTheSize = [&inputBuf]()
			{
				Logger::WriteMessage("asserting size > 0");
				Assert::IsTrue(inputBuf.size() >0);
				Logger::WriteMessage("size > 0 asserted true");
			};
			//assert >0 results
			inputBuf = mover.GetPackagedInput(PixelsToMoveHigh, PixelsToMoveHigh);
			assertTheSize();
			testInputBuf(true,false,L"[1]");
			inputBuf = mover.GetPackagedInput(PixelsToMoveLow, PixelsToMoveLow);
			assertTheSize();
			testInputBuf(true,false, L"[2]");
			//assert >=0 results
			inputBuf = mover.GetPackagedInput(PixelsToMoveLow, PixelsToMoveHigh);
			assertTheSize();
			testInputBuf(true,true, L"[3]");
			inputBuf = mover.GetPackagedInput(PixelsToMoveHigh, PixelsToMoveLow);
			assertTheSize();
			testInputBuf(true,true, L"[4]");

			//assert <0 results
			inputBuf = mover.GetPackagedInput(-(PixelsToMoveHigh), -(PixelsToMoveHigh));
			assertTheSize();
			testInputBuf(false,false, L"[5]");
			inputBuf = mover.GetPackagedInput(-(PixelsToMoveLow), -(PixelsToMoveLow));
			assertTheSize();
			testInputBuf(false, false, L"[6]");
			inputBuf = mover.GetPackagedInput(-(PixelsToMoveLow), -(PixelsToMoveHigh));
			assertTheSize();
			testInputBuf(false, true, L"[7]");
			inputBuf = mover.GetPackagedInput(-(PixelsToMoveHigh), -(PixelsToMoveLow));
			assertTheSize();
			testInputBuf(false, true, L"[8]");

			Logger::WriteMessage("End TestGetPackagedInput()");
		}

		TEST_METHOD(TestGetFinalInput)
		{
			Logger::WriteMessage("Begin TestGetFinalInput()");
			sds::ThumbstickToMovement mover(SENS_VAL, DEADZONE);

			//vector to hold the results
			std::vector<std::tuple<int, int>> inputBuf;
			//lambda to assert the size of the returned vector > 0
			auto assertTheSize = [&inputBuf]()
			{
				Logger::WriteMessage("asserting size > 0");
				Assert::IsTrue(inputBuf.size() > 0);
				Logger::WriteMessage("size > 0 asserted true");
			};
			//main lambda to run a test
			auto testInputBuf = [&inputBuf](bool firstValGreater, bool secondValGreater, bool firstUseEqual, bool secondUseEqual, std::wstring testNumber)
			{
				//helper to build a message from a tuple
				auto getMessage = [](std::tuple<int, int> t)
				{
					std::wstring mes(std::to_wstring(std::get<0>(t)) + L" and " + std::to_wstring(std::get<1>(t)));
					return mes;
				};
				//lambdas to test each element with
				std::wstring message;
				auto testFirstGreaterEqual = [&message, &testNumber, &getMessage](std::tuple<int, int> &t)
				{
					message = testNumber + L" :Begin testing first greater than equal zero. ";
					message += getMessage(t);
					Assert::IsTrue((std::get<0>(t) >= 0), message.c_str());
				};
				auto testFirstLessEqual = [&message, &testNumber, &getMessage](std::tuple<int, int> &t)
				{
					message = testNumber + L" :Begin testing first less than equal zero. ";
					message += getMessage(t);
					Assert::IsTrue((std::get<0>(t) <= 0),message.c_str());
				};
				auto testFirstGreater = [&message, &testNumber, &getMessage](std::tuple<int, int> &t)
				{
					message = testNumber + L" :Begin testing first greater than zero. ";
					message += getMessage(t);
					Assert::IsTrue((std::get<0>(t) > 0), message.c_str());
				};
				auto testFirstLess = [&message, &testNumber, &getMessage](std::tuple<int, int> &t)
				{
					message = testNumber + L" :Begin testing first less than zero. ";
					message += getMessage(t);
					Assert::IsTrue((std::get<0>(t) < 0),message.c_str());
				};
				auto testSecondGreaterEqual = [&message,&testNumber, &getMessage](std::tuple<int, int> &t)
				{
					message = testNumber + L" :Begin testing second greater than equal zero. ";
					message += getMessage(t);
					Assert::IsTrue((std::get<1>(t) >= 0),message.c_str());
				};
				auto testSecondLessEqual = [&message, &testNumber, &getMessage](std::tuple<int, int> &t)
				{
					message = testNumber + L" :Begin testing second less than equal zero. ";
					message += getMessage(t);
					Assert::IsTrue((std::get<1>(t) <= 0),message.c_str());
				};
				auto testSecondGreater = [&message, &testNumber, &getMessage](std::tuple<int, int> &t)
				{
					message = testNumber + L" :Begin testing second greater than zero. ";
					message += getMessage(t);
					Assert::IsTrue((std::get<1>(t) > 0),message.c_str());
				};
				auto testSecondLess = [&message, &testNumber, &getMessage](std::tuple<int, int> &t)
				{
					message = testNumber + L" :Begin testing second less than zero. ";
					message += getMessage(t);
					Assert::IsTrue((std::get<1>(t) < 0),message.c_str());
				};

				//Run the tests for the first elements
				if (firstValGreater && firstUseEqual)
				{
					std::for_each(inputBuf.begin(), inputBuf.end(), testFirstGreaterEqual);
				}
				else if (firstValGreater && !firstUseEqual)
				{
					std::for_each(inputBuf.begin(), inputBuf.end(), testFirstGreater);
				}
				else if (!firstValGreater && firstUseEqual)
				{
					std::for_each(inputBuf.begin(), inputBuf.end(), testFirstLessEqual);
				}
				else if (!firstValGreater && !firstUseEqual)
				{
					std::for_each(inputBuf.begin(), inputBuf.end(), testFirstLess);
				}

				//Run the tests for the second elements
				if (secondValGreater && secondUseEqual)
				{
					std::for_each(inputBuf.begin(), inputBuf.end(), testSecondGreaterEqual);
				}
				else if (secondValGreater && !secondUseEqual)
				{
					std::for_each(inputBuf.begin(), inputBuf.end(), testSecondGreater);
				}
				else if (!secondValGreater && secondUseEqual)
				{
					std::for_each(inputBuf.begin(), inputBuf.end(), testSecondLessEqual);
				}
				else if (!secondValGreater && !secondUseEqual)
				{
					std::for_each(inputBuf.begin(), inputBuf.end(), testSecondLess);
				}
			};

			//tests for the case of both elements being 0
			auto testBothNotZero = [&inputBuf](std::string testNumber)
			{
				std::string message;
				message = testNumber + " :Begin testing both not zero. ";
				Logger::WriteMessage(message.c_str());
				std::for_each(inputBuf.begin(), inputBuf.end(), [](std::tuple<int, int> &t)
					{
						bool val = std::get<0>(t) != 0;
						val = val || std::get<1>(t) != 0;
						Assert::IsTrue(val);
					});
				
				message = testNumber + " :End testing both not zero. ";
				Logger::WriteMessage(message.c_str());
			};

			//test lambda calls
			inputBuf = mover.GetFinalInput(SMax, SMax);
			assertTheSize();
			testInputBuf(true, false, false, false, L"[1]");
			testBothNotZero("[2]");

			inputBuf = mover.GetFinalInput(SMin, SMin);
			assertTheSize();
			testInputBuf(false, true, false, false, L"[3]");
			testBothNotZero("[4]");

			inputBuf = mover.GetFinalInput(SMin, SMax);
			assertTheSize();
			testInputBuf(false, false, false, false, L"[5]");
			testBothNotZero("[6]");

			inputBuf = mover.GetFinalInput(SMax, SMin);
			assertTheSize();
			testInputBuf(true, true, false, false, L"[7]");
			testBothNotZero("[8]");

			//test many in a loop
			Logger::WriteMessage("Starting loop testing.");
			for (int i = SMax, j = SMin; i > 0 && j < SMax; i--, j++)
			{
				std::wstring curVal = L"[TestGetFinalInput]:[9]= ";
				curVal += std::to_wstring(i) + L" : " + std::to_wstring(j);
				inputBuf = mover.GetFinalInput(i, j);
				testInputBuf(true, true, true, true, curVal);
			}

			Logger::WriteMessage("Finished loop testing.");
			Logger::WriteMessage("End TestGetFinalInput()");
		}
	};
}

