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
		std::atomic<MouseMap> m_stickMapInfo;
		std::atomic<SHORT> m_threadX, m_threadY;
		std::atomic<int> m_mouseSensitivity;

		sds::PlayerInfo m_localPlayerInfo;
	public:
		/// <summary>
		/// Ctor for default configuration
		/// </summary>
		XInputBoostMouse()
			: CPPThreadRunner(),
			m_stickMapInfo(MouseMap::NEITHER_STICK),
			m_mouseSensitivity(XinSettings::SENSITIVITY_DEFAULT)
		{
			m_threadX = 0;
			m_threadY = 0;
		}
		/// <summary>
		/// Ctor allows setting a custom PlayerInfo
		/// </summary>
		XInputBoostMouse(const sds::PlayerInfo &player)
			: CPPThreadRunner(),
			m_stickMapInfo(MouseMap::NEITHER_STICK),
			m_mouseSensitivity(XinSettings::SENSITIVITY_DEFAULT)
		{
			m_localPlayerInfo = player;
			m_threadX = 0;
			m_threadY = 0;
		}
		XInputBoostMouse(const XInputBoostMouse& other) = delete;
		XInputBoostMouse(XInputBoostMouse&& other) = delete;
		XInputBoostMouse& operator=(const XInputBoostMouse& other) = delete;
		XInputBoostMouse& operator=(XInputBoostMouse&& other) = delete;
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
		void EnableProcessing(const MouseMap info)
		{
			if(this->isThreadRunning && !this->isStopRequested)
			{
				m_stickMapInfo = info;
				this->startThread();
			}
			else if(this->isStopRequested)
			{
				this->stopThread();
				m_stickMapInfo = info;
				this->startThread();
			}
		}
		/// <summary>
		/// Called to check XINPUT_STATE values for mouse movement requirements.
		/// Will start the workThread running if required.
		/// </summary>
		/// <param name="state"> an XINPUT_STATE </param>
		void ProcessState(const XINPUT_STATE &state)
		{
			if(m_stickMapInfo == MouseMap::NEITHER_STICK)
				return;
			//Holds the reported stick values and will compare to determine if movement has occurred.
			int tsx, tsy;
		
			if(m_stickMapInfo == MouseMap::RIGHT_STICK)
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
			m_threadX = static_cast<SHORT>(tsx);
			m_threadY = static_cast<SHORT>(tsy);

			const int tdx = m_stickMapInfo == MouseMap::RIGHT_STICK ? m_localPlayerInfo.right_x_dz : m_localPlayerInfo.left_x_dz;
			const int tdy = m_stickMapInfo == MouseMap::RIGHT_STICK ? m_localPlayerInfo.right_y_dz : m_localPlayerInfo.left_y_dz;

			this->updateState(state);
			if(!this->isThreadRunning)
				this->startThread();
		}
		/// <summary>
		/// Setter for sensitivity value, blocks while work thread stops and restarts.
		/// </summary>
		/// <param name="new_sens"></param>
		/// <returns> returns a std::string containing an error message
		/// if there is an error, empty string otherwise. </returns>
		std::string SetSensitivity(const int new_sens)
		{
			if (new_sens < XinSettings::SENSITIVITY_MIN || new_sens > XinSettings::SENSITIVITY_MAX)
			{
				return "Error in sds::XInputBoostMouse::SetSensitivity(), int new_sens out of range.";
			}
			m_mouseSensitivity = new_sens;
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
			return m_mouseSensitivity;
		}
	private:
		/// <summary>
		/// Worker thread, private visibility, gets updated data from ProcessState() function to use.
		/// Accesses the std::atomic m_threadX and m_threadY members.
		/// </summary>
		void workThread() override
		{
			ThumbstickAxisThread xThread(this->GetSensitivity(), m_localPlayerInfo, m_stickMapInfo, true);
			ThumbstickAxisThread yThread(this->GetSensitivity(), m_localPlayerInfo, m_stickMapInfo, false);
			
			xThread.Start();
			yThread.Start();

			//thread main loop
			while(!isStopRequested)
			{
				xThread.ProcessState(m_threadX, m_threadY);
				yThread.ProcessState(m_threadX, m_threadY);
				
				std::this_thread::sleep_for(std::chrono::microseconds(XinSettings::MOVE_THREAD_SLEEP_MICRO));
			}
			xThread.Stop();
			yThread.Stop();
			//mark thread status as not running.
			isThreadRunning = false;
		}

	};
}