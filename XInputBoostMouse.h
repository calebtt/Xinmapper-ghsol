/*
Further design considerations would include
interpreting an ActionString,
and to allow mapping with the MapString.
Which would mean updating ActionDescriptors to include the new
tokens, and updating this class to interpret an ActionString,
and finally update GamepadUser.
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

		sds::PlayerInfo localPlayerInfo;

		//default vals
		const int SENSITIVITY_DEFAULT = 35;
		const float MULTIPLIER_DEFAULT = 0.5f;
		//constants
		const int MOVE_THREAD_SLEEP = 4;//4 ms
		const int MOVE_THREAD_SLEEP_MOVE = 2;//2 ms
		const int MOVE_THREAD_SLEEP_MICRO = 4000; //4 milliseconds
		const int SENSITIVITY_MIN = 1;
		const int SENSITIVITY_MAX = 100;
		const float MULTIPLIER_MIN = 0.01f;
		const float MULTIPLIER_MAX = 1.0f;
	public:
		/// <summary>
		/// Ctor for default configuration
		/// </summary>
		XInputBoostMouse()
			: CPPThreadRunner(),
			stickMapInfo(MouseMap::NEITHER_STICK),
			mouseSensitivity(SENSITIVITY_DEFAULT),
			isDeadzoneActivated(false), 
			altDeadzoneConfig(true), 
			altDeadzoneMultiplier(MULTIPLIER_DEFAULT)
		{
			threadX = 0;
			threadY = 0;
		}
		/// <summary>
		/// Ctor allows setting a custom PlayerInfo
		/// </summary>
		XInputBoostMouse(const sds::PlayerInfo &player)
			: CPPThreadRunner(),
			stickMapInfo(MouseMap::NEITHER_STICK),
			mouseSensitivity(SENSITIVITY_DEFAULT),
			isDeadzoneActivated(false), 
			altDeadzoneConfig(true), 
			altDeadzoneMultiplier(MULTIPLIER_DEFAULT)
		{
			localPlayerInfo = player;
			threadX = 0;
			threadY = 0;
		}

		/// <summary>
		/// Destructor override, ensures the running thread function is stopped
		/// inside of this class and not the base.
		/// </summary>
		~XInputBoostMouse() override
		{ 
			this->stopThread();
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


			int tdx = stickMapInfo == MouseMap::RIGHT_STICK ? localPlayerInfo.right_x_dz : localPlayerInfo.left_x_dz;
			int tdy = stickMapInfo == MouseMap::RIGHT_STICK ? localPlayerInfo.right_y_dz : localPlayerInfo.left_y_dz;
			ThumbstickToDelay moveDetermine(mouseSensitivity, tdx, tdy);

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
			if (new_sens < SENSITIVITY_MIN || new_sens > SENSITIVITY_MAX)
			{
				return "Error in sds::XInputBoostMouse::SetSensitivity(), int new_sens out of range.";
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
			int dzx = this->stickMapInfo == MouseMap::RIGHT_STICK ? localPlayerInfo.right_x_dz : localPlayerInfo.left_x_dz;
			int dzy = this->stickMapInfo == MouseMap::RIGHT_STICK ? localPlayerInfo.right_y_dz : localPlayerInfo.left_y_dz;

			ThumbstickAxisThread xThread(this->GetSensitivity(), localPlayerInfo, stickMapInfo, true);
			ThumbstickAxisThread yThread(this->GetSensitivity(), localPlayerInfo, stickMapInfo, false);
			
			xThread.Start();
			yThread.Start();

			//thread main loop
			while(!isStopRequested)//<--Danger! From the base class.
			{
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