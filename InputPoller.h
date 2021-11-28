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
		Mapper *m;
		XInputTranslater *t;
		XInputBoostMouse *mse;
		PlayerInfo localPlayer;

	protected://member functions
		/// <summary>
		/// Worker thread overriding the base pure virtual workThread,
		/// uses a sds::Mapper, sds::XInputTranslater, sds::XInputBoostMouse
		/// runs a constant loop of getting state information in the form of an XINPUT_STATE
		/// it then processes with either the XInputBoostMouse or the Mapper.
		/// </summary>
		void workThread() override
		{
			//creating a local scope for a scoped lock to protect access to the local_state
			{
				lock l(this->stateMutex);
				memset(&local_state, 0, sizeof(local_state));
			}

			while( ! this->isStopRequested )
			{	

				{
					lock l2(this->stateMutex);
					DWORD error = XInputGetState(localPlayer.player_id, &local_state);
					if (error != ERROR_SUCCESS)
					{
						continue;
					}
				}

				mse->ProcessState(this->getCurrentState());
				m->ProcessActionDetails(t->ProcessState(this->getCurrentState()));

				std::this_thread::sleep_for(std::chrono::milliseconds(XinSettings::THREAD_DELAY_POLLER));
			}
			this->isThreadRunning = false;
		}

	public:
		/// <summary>
		/// Constructor, requires pointer to: Mapper, XInputTranslater, XInputBoostMouse
		/// Throws std::string with error message if nullptr given.
		/// </summary>
		/// <exception cref="std::string">throws a string if null pointers are given to the ctor.</exception>
		/// <param name="mapper"></param>
		/// <param name="transl"></param>
		/// <param name="mouse"></param>
		InputPoller(Mapper *mapper, XInputTranslater *transl, XInputBoostMouse *mouse)
			: CPPThreadRunner(), m(mapper), t(transl), mse(mouse)
		{
			if (mapper == nullptr || transl == nullptr || mouse == nullptr)
			{
				//At present this new exception type is only available in the preview features "C++ latest" setting.
				//throw std::format_error("NULL POINTERS in InputPoller::InputPoller() constructor.");
				throw std::string("NULL POINTERS in InputPoller::InputPoller() constructor.");
			}
		}

		/// <summary>
		/// Constructor, requires pointer to: Mapper, XInputTranslater, XInputBoostMouse
		/// Throws std::string with error message if nullptr given.
		/// </summary>
		/// 
		/// <param name="mapper"></param>
		/// <param name="transl"></param>
		/// <param name="mouse"></param>
		/// <param name="p"></param>
		InputPoller(Mapper *mapper, XInputTranslater *transl, XInputBoostMouse *mouse, const PlayerInfo &p)
			: CPPThreadRunner(), m(mapper), t(transl), mse(mouse)
		{
			if (mapper == nullptr || transl == nullptr || mouse == nullptr)
			{
				//At present this new exception type is only available in the preview features "C++ latest" setting.
				//throw std::format_error("NULL POINTERS in InputPoller::InputPoller() constructor.");
				throw std::string("NULL POINTERS in InputPoller::InputPoller() constructor.");
			}
			localPlayer = p;
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
			return XInputGetState(localPlayer.player_id, &ss) == ERROR_SUCCESS;
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