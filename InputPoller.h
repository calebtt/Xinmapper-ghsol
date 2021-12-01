#pragma once
#include "stdafx.h"
#include "Mapper.h"
#include "XInputTranslater.h"
#include "XInputBoostMouse.h"
#include "CPPThreadRunner.h"

namespace sds
{
	/// <summary>
	/// Polls for input from the XInput library in it's worker thread function,
	/// sends them to XInputBoostMouse and Mapper for processing.
	/// </summary>
	class InputPoller : public CPPThreadRunner<XINPUT_STATE>
	{
		Mapper &m_mapper;
		XInputTranslater &m_translater;
		XInputBoostMouse &m_mouse;
		PlayerInfo m_localPlayer;
	protected:
		/// <summary>
		/// Worker thread overriding the base pure virtual workThread,
		/// uses a sds::Mapper, sds::XInputTranslater, sds::XInputBoostMouse
		/// runs a constant loop of getting state information in the form of an XINPUT_STATE
		/// it then processes with either the XInputBoostMouse or the Mapper.
		/// </summary>
		void workThread() override
		{
			//because there is only one thread modifying the local_state struct, we won't use the mutex.
			local_state = {};
			while( ! this->isStopRequested )
			{	
				const DWORD error = XInputGetState(m_localPlayer.player_id, &local_state);
				if (error != ERROR_SUCCESS)
				{
					continue;
				}
				m_mouse.ProcessState(local_state);
				m_mapper.ProcessActionDetails(m_translater.ProcessState(local_state));
				std::this_thread::sleep_for(std::chrono::milliseconds(XinSettings::THREAD_DELAY_POLLER));
			}
			this->isThreadRunning = false;
		}

	public:
		/// <summary>
		/// Constructor, requires ref to objects: Mapper, XInputTranslater, XInputBoostMouse
		/// </summary>
		/// <param name="mapper"></param>
		/// <param name="transl"></param>
		/// <param name="mouse"></param>
		InputPoller(Mapper &mapper, XInputTranslater &transl, XInputBoostMouse &mouse)
			: CPPThreadRunner(), m_mapper(mapper), m_translater(transl), m_mouse(mouse)
		{
		}

		/// <summary>
		/// Alt constructor, requires ref to objects: Mapper, XInputTranslater, XInputBoostMouse
		///	and a PlayerInfo object.
		/// </summary>
		/// <param name="mapper"></param>
		/// <param name="transl"></param>
		/// <param name="mouse"></param>
		/// <param name="p">custom playerinfo object</param>
		InputPoller(Mapper &mapper, XInputTranslater &transl, XInputBoostMouse &mouse, const PlayerInfo &p)
			: CPPThreadRunner(), m_mapper(mapper), m_translater(transl), m_mouse(mouse)
		{
			m_localPlayer = p;
		}

		/// <summary>
		/// Explicitly deleted C++11 default constructor, intent is known there is no default constructor used here.
		/// </summary>
		InputPoller() = delete;

		
		/// <summary>
		/// Destructor override, ensures the running thread function is stopped
		/// inside of this class and not the base.
		/// </summary>
		~InputPoller() override
		{
			this->stopThread();
		}

		/// <summary>
		/// Start polling for input (and processing via Mapper, XInputBoostMouse, XInputTranslater)
		/// </summary>
		/// <returns> true if thread started running (or was already running) </returns>
		bool Start()
		{
			this->startThread();
			return this->isThreadRunning;
		}

		/// <summary>
		/// Stop input polling.
		/// </summary>
		/// <returns>false is thread has stopped running, true if it failed</returns>
		bool Stop()
		{
			this->stopThread();
			return this->isThreadRunning;
		}

		/// <summary>
		/// Gets the running status of the worker thread
		/// </summary>
		/// <returns> true if thread is running, false otherwise</returns>
		bool IsRunning() const
		{
			return this->isThreadRunning;
		}
		/// <summary>
		/// Returns status of XINPUT library detecting a controller.
		/// </summary>
		/// <returns> true if controller is connected, false otherwise</returns>
		bool IsControllerConnected() const
		{
			XINPUT_STATE ss = {};
			return XInputGetState(m_localPlayer.player_id, &ss) == ERROR_SUCCESS;
		}
		/// <summary>
		/// Returns status of XINPUT library detecting a controller.
		/// overload that uses the player_id value in a PlayerInfo struct
		/// </summary>
		/// <returns> true if controller is connected, false otherwise</returns>
		bool IsControllerConnected(const PlayerInfo &p) const
		{
			XINPUT_STATE ss = {};
			return XInputGetState(p.player_id, &ss) == ERROR_SUCCESS;
		}
	};

}