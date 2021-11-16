/*
Further design considerations would include
interpreting an ActionString,
and to allow mapping with the MapString.
Which would mean updating ActionDescriptors to include the new
tokens, and updating this class to interpret an ActionString,
and finally update GamepadUser.

Finally, the ability to modify the sensitivity function
is of utmost importance, which may require a math expression
parser.  The class currently uses the graph of [ y = (x^2) / mouseSensitivity + 1 ]
which is a parabola with the quality that at the curve's highest points
it is the nearly (or exactly) the size of the viewing window--the mouseSensitivity.
Also need sensitivity for both X and Y axis.
*/

#pragma once
#include "stdafx.h"
#include "CPPThreadRunner.h"
#include "ThumbstickToMovement.h"
#include "ThumbstickAxisThread.h"

namespace sds
{
	/// <summary>
	/// Handles achieving smooth, expected mouse movements.
	/// The class holds info on which mouse stick (if any) is to be used for controlling the mouse,
	/// the MouseMap enum holds this info.
	/// This class starts a running thread that is used only to process the XINPUT_STATE structure
	/// and use those values to determine if it should move the mouse cursor, and if so how much.
	/// Another thread calls ProcessState(XINPUT_STATE) to update the internal XINPUT_STATE struct.
	/// It also has public functions for getting and setting the sensitivity.
	/// </summary>
	class XInputBoostMouse : public CPPThreadRunner<XINPUT_STATE>
	{
	private:
		std::atomic<MouseMap> stickMapInfo;
		std::atomic<SHORT> threadX, threadY;
		std::atomic<int> mouseSensitivity;
		std::atomic<bool> isDeadzoneActivated;
		std::atomic<bool> altDeadzoneConfig;
		std::atomic<float> altDeadzoneMultiplier;

		int playerLeftDeadzone;
		int playerRightDeadzone;

		const int MOVE_THREAD_SLEEP = 4;//4 ms
		const int MOVE_THREAD_SLEEP_MOVE = 2;//2 ms
		const int MOVE_THREAD_SLEEP_MICRO = 4000; //4000 microseconds
		//const int MOVE_THREAD_SLEEP_MICRO = 4000000;
		const int SENSITIVITY_MIN = 0;
		const int SENSITIVITY_MAX = 100;
		const float MULTIPLIER_MIN = 0.01f;
		const float MULTIPLIER_MAX = 1.0F;
	public:
		/// <summary>
		/// Ctor Initializes some configuration variables like the alternate deadzone config multiplier
		/// </summary>
		XInputBoostMouse()
			: CPPThreadRunner(),
			mouseSensitivity(30),
			stickMapInfo(MouseMap::NEITHER_STICK),
			isDeadzoneActivated(false), altDeadzoneConfig(true), altDeadzoneMultiplier(0.5f),
			playerLeftDeadzone(sds::sdsPlayerOne.left_dz), playerRightDeadzone(sds::sdsPlayerOne.right_dz)
		{
		}
		/// <summary>
		/// Ctor Initializes some configuration variables like the alternate deadzone config multiplier
		/// and allows setting a custom PlayerInfo
		/// </summary>
		XInputBoostMouse(const sds::PlayerInfo &player)
			: CPPThreadRunner(),
			mouseSensitivity(30),
			stickMapInfo(MouseMap::NEITHER_STICK),
			isDeadzoneActivated(false), altDeadzoneConfig(true), altDeadzoneMultiplier(0.5f)
		{
			playerLeftDeadzone = player.left_dz;
			playerRightDeadzone = player.right_dz;
			threadX = 0;
			threadY = 0;
		}
		~XInputBoostMouse()
		{ 
			//this->HaltOperation(); 
		}
		/// <summary>
		/// Use this function to establish one stick or the other as the one controlling the mouse movements.
		/// Set to NEITHER_STICK for no thumbstick mouse movement. Options are RIGHT_STICK, LEFT_STICK, NEITHER_STICK
		/// </summary>
		/// <param name="info"> a MouseMap enum</param>
		void EnableProcessing(MouseMap info)
		{
			stickMapInfo = info;
		}

		/// <summary>
		/// Called to check XINPUT_STATE values for mouse movement requirements.
		/// Will start the workThread running if required.
		/// </summary>
		/// <param name="state"> an XINPUT_STATE </param>
		void ProcessState(const XINPUT_STATE &state)
		{
			if(stickMapInfo == MouseMap::NEITHER_STICK)
				return;
			//Holds the reported stick values and will compare to determine if movement has occurred.
			int tsx, tsy;
		
			if(stickMapInfo == MouseMap::RIGHT_STICK)
			{
				tsx = state.Gamepad.sThumbRX;
				tsy = state.Gamepad.sThumbRY;
			}
			else
			{
				tsx = state.Gamepad.sThumbLX;
				tsy = state.Gamepad.sThumbLY;
			}
			//Give worker thread new values.
			threadX = tsx;
			threadY = tsy;

			int tdz = stickMapInfo == MouseMap::RIGHT_STICK ? sdsPlayerOne.right_dz : sdsPlayerOne.left_dz;
			ThumbstickToMovement moveDetermine(this->mouseSensitivity, tdz, this->altDeadzoneMultiplier);

			if( moveDetermine.DoesRequireMove(tsx,tsy) )
			{
				//update state.
				this->updateState(state);

				//check for thread is running.
				if( !isThreadRunning )
					this->startThread();
			}
		}

		/// <summary>
		/// Setter for telling the processing to use the alternate deadzone processing config.
		/// </summary>
		/// <param name="useAlt"></param>
		void SetUseAltDeadzone(bool useAlt)
		{
			altDeadzoneConfig = useAlt;
		}
		/// <summary>
		/// Getter for current status of using the alternate deadzone processing config.
		/// </summary>
		/// <returns></returns>
		bool GetUseAltDeadzone() const
		{
			return altDeadzoneConfig;
		}
		/// <summary>
		/// Sets the value of the alternate deadzone multiplier.
		/// For the behavior of the alternate deadzone configuration,
		/// the multiplier is used to lower the deadzone when one is already activated.
		/// </summary>
		/// <param name="newValue"></param>
		/// <returns></returns>
		std::string SetAltDeadzoneMultiplier(float newValue)
		{
			if (newValue > MULTIPLIER_MAX|| newValue < MULTIPLIER_MIN)
				return "Failed to set new deadzone value, out of range.";
			altDeadzoneMultiplier = newValue;
			this->stopThread();
			this->startThread();
			return "";
		}

		/// <summary>
		/// Gets the current alternate deadzone multiplier.
		/// </summary>
		/// <returns></returns>
		float GetAltDeadzoneMultiplier() const
		{
			float r = altDeadzoneMultiplier;
			return r;
		}
		/// <summary>
		/// Setter for sensitivity value, blocks while work thread stops and restarts.
		/// </summary>
		/// <param name="new_sens"></param>
		/// <returns> returns a std::string containing an error message
		/// if there is an error, empty string otherwise. </returns>
		std::string SetSensitivity(int new_sens)
		{
			if (new_sens <= SENSITIVITY_MIN || new_sens > SENSITIVITY_MAX)
			{
				return "Error in sds::XInputBoostMouse::SetSensitivity(), int new_sens less than or equal to 0 or greater than 100.";
			}
			mouseSensitivity = new_sens;
			this->stopThread();
			this->startThread();
			return "";
		}
		/// <summary>
		/// Getter for sensitivity value
		/// </summary>
		/// <returns></returns>
		int GetSensitivity() const
		{
			return mouseSensitivity;
		}
	private:
		/// <summary>
		/// Worker thread, private visibility, gets updated data from ProcessState() function to use.
		/// Accesses the std::atomic threadX and threadY members.
		/// </summary>
		virtual void workThread()
		{
			int dzz = this->stickMapInfo == MouseMap::RIGHT_STICK ? this->playerRightDeadzone : this->playerLeftDeadzone;
			ThumbstickAxisThread xThread(dzz, this->GetSensitivity(), true);
			ThumbstickAxisThread yThread(dzz, this->GetSensitivity(), false);
			xThread.Start();
			yThread.Start();
			//int tx = threadX;
			//int ty = threadY;
			//std::thread t2 = yThread.GetProcessStateThread(ty);
			//std::thread t1 = xThread.GetProcessStateThread(tx);
			//thread main loop
			while(!isStopRequested)//<--Danger! From the base class.
			{
				//if (t2.joinable())
				//	t2.join();
				//if (t1.joinable())
				//	t1.join();
				//delegate values to separate threads
				//create two threads to call the update function asynchronously
				//t2 = yThread.GetProcessStateThread(ty);
				//t1 = xThread.GetProcessStateThread(tx);
				xThread.ProcessState(threadX);
				yThread.ProcessState(threadY);
				
				std::this_thread::sleep_for(std::chrono::microseconds(MOVE_THREAD_SLEEP_MICRO));
			}
			xThread.Stop();
			yThread.Stop();
			//mark thread status as not running.
			isThreadRunning = false;
		}

	};
}