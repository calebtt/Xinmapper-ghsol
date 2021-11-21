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

		sds::PlayerInfo localPlayerInfo;
	public:
		/// <summary>
		/// Ctor for default configuration
		/// </summary>
		XInputBoostMouse()
			: CPPThreadRunner(),
			stickMapInfo(MouseMap::NEITHER_STICK),
			mouseSensitivity(XinSettings::SENSITIVITY_DEFAULT)
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
			mouseSensitivity(XinSettings::SENSITIVITY_DEFAULT)
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
		/// Setter for sensitivity value, blocks while work thread stops and restarts.
		/// </summary>
		/// <param name="new_sens"></param>
		/// <returns> returns a std::string containing an error message
		/// if there is an error, empty string otherwise. </returns>
		std::string SetSensitivity(int new_sens)
		{
			if (new_sens < XinSettings::SENSITIVITY_MIN || new_sens > XinSettings::SENSITIVITY_MAX)
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
				xThread.ProcessState(threadX, threadY);
				yThread.ProcessState(threadX, threadY);
				
				std::this_thread::sleep_for(std::chrono::microseconds(XinSettings::MOVE_THREAD_SLEEP_MICRO));
			}
			xThread.Stop();
			yThread.Stop();
			//mark thread status as not running.
			isThreadRunning = false;
		}

	};
}