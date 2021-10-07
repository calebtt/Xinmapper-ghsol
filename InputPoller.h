#pragma once
#include "stdafx.h"
#include "Mapper.h"
#include "XInputTranslater.h"
#include "XInputBoostMouse.h"

namespace sds
{
	/// <summary>
	/// Polls for input from the XInput library in it's worker thread function,
	/// sends them to XInputBoostMouse and Mapper for processing.
	/// </summary>
	class InputPoller : public XInputBoostAsync
	{
		const int THREAD_DELAY;
		Mapper *m;
		XInputTranslater *t;
		XInputBoostMouse *mse;

	protected://member functions
		/// <summary>
		/// Worker thread overriding the base pure virtual workThread,
		/// uses a sds::Mapper, sds::XInputTranslater, sds::XInputBoostMouse
		/// runs a constant loop of getting state information in the form of an XINPUT_STATE
		/// it then processes with either the XInputBoostMouse or the Mapper.
		/// </summary>
		virtual void workThread()
		{
			while( ! this->isStopRequested )
			{
				DWORD error = XInputGetState(sds::sdsPlayerOne.player_id, &local_state);
				if( error != ERROR_SUCCESS )
					break;
				mse->ProcessState(local_state);
				m->ProcessActionDetails( t->ProcessState(local_state) );
				Sleep(THREAD_DELAY);
			}
			this->isThreadRunning = false;
		}

	public:
		/// <summary>
		/// Constructor, requires pointer to: Mapper, XInputTranslater, XInputBoostMouse
		/// </summary>
		/// <param name="mapper"></param>
		/// <param name="transl"></param>
		/// <param name="mouse"></param>
		InputPoller(Mapper *mapper, XInputTranslater *transl, XInputBoostMouse *mouse)
			: XInputBoostAsync(), THREAD_DELAY(10), m(mapper), t(transl), mse(mouse)
		{
		}

		/// <summary>
		/// Explicitly deleted C++11 default constructor, intent is known there is no default constructor used here.
		/// </summary>
		InputPoller() = delete;


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
		bool IsRunning()
		{
			return this->isThreadRunning;
		}
		/// <summary>
		/// Returns status of XINPUT library detecting a controller.
		/// </summary>
		/// <returns> true if controller is connected, false otherwise</returns>
		bool IsControllerConnected()
		{
			XINPUT_STATE ss = {0};
			return XInputGetState(sds::sdsPlayerOne.player_id, &ss) == ERROR_SUCCESS;
		}
		/// <summary>
		/// Unimplemented. Do not use.
		/// </summary>
		/// <param name="state"></param>
		void ProcessState(const XINPUT_STATE &state)
		{
			throw sds::ERR1;
		}
	};

}