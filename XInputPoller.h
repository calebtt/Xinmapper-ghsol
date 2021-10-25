#pragma once
#include "stdafx.h"
#include "Mapper.h"
#include "XInputTranslater.h"
#include "XInputMouse.h"
#include "CPPThreadRunner.h"
#include "InputPollerBase.h"

namespace sds
{
	/// <summary>
	/// Polls for input from the XInput library in it's worker thread function,
	/// sends them to XInputBoostMouse and Mapper for processing.
	/// </summary>
	class XInputPoller : public InputPollerBase<XINPUT_STATE>
	{
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

			while (!this->isStopRequested)
			{
				{
					lock l2(this->stateMutex);
					DWORD error = XInputGetState(sds::sdsPlayerOne.player_id, &local_state);
					if (error != ERROR_SUCCESS)
					{
						continue;
					}
				}

				mse->ProcessState(this->getCurrentState());
				m->ProcessActionDetails(t->ProcessState(this->getCurrentState()));

				Sleep(THREAD_DELAY);
			}
			this->isThreadRunning = false;
		}

	public:
		/// <summary>
		/// Constructor, requires pointer to: Mapper, XInputTranslater, XInputBoostMouse
		/// Base throws std::string with error message if nullptr given.
		/// </summary>
		/// 
		/// <param name="mapper"></param>
		/// <param name="transl"></param>
		/// <param name="mouse"></param>
		XInputPoller(Mapper* mapper, InputTranslaterBase<XINPUT_STATE> *transl, InputMouseBase<XINPUT_STATE> *mouse)
			: InputPollerBase(mapper, transl, mouse)
		{
		}

		/// <summary>
		/// Explicitly deleted C++11 default constructor, intent is known there is no default constructor used here.
		/// </summary>
		XInputPoller() = delete;

		/// <summary>
		/// Returns status of XINPUT library detecting a controller.
		/// </summary>
		/// <returns> true if controller is connected, false otherwise</returns>
		bool IsControllerConnected() override
		{
			XINPUT_STATE ss = { 0 };
			return XInputGetState(sds::sdsPlayerOne.player_id, &ss) == ERROR_SUCCESS;
		}
	};

}