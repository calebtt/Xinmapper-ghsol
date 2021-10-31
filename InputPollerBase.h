#pragma once
#include "stdafx.h"
#include "Mapper.h"
#include "XInputTranslater.h"
#include "XInputMouse.h"
#include "CPPThreadRunner.h"

namespace sds
{
	/// <summary>
	/// Polls for input from the [Input] library in it's worker thread function,
	/// derived workThread() sends them to Mapper, Translater, XInputBoostMouse for processing.
	/// </summary>
	template <class InputLibType>
	class InputPollerBase : public CPPThreadRunner<InputLibType>
	{
	protected:
		const int THREAD_DELAY = 10;
		Mapper *m;
		InputTranslaterBase<InputLibType> *t;
		InputMouseBase<InputLibType> *mse;

		/// <summary>
		/// Pure virtual workThread(), override in derived classes.
		/// Intended to poll for input and use: Mapper, Translater, XInputBoostMouse
		/// for the input processing.
		/// </summary>
		virtual void workThread() = 0;

	public:

		/// <summary>
		/// Pure virtual IsControllerConnected(), override in derived class.
		/// Returns status of [Input] library detecting a controller.
		/// </summary>
		/// <returns> true if controller is connected, false otherwise</returns>
		virtual bool IsControllerConnected() = 0;

		/// <summary>
		/// Constructor, requires pointer to: Mapper, XInputTranslater, XInputBoostMouse
		/// Throws std::string with error message if nullptr given.
		/// DOES NOT ACQUIRE OWNERSHIP of the pointers, merely uses them.
		/// Precondition: pointers point to valid instances
		/// Postcondition: pointers not deleted
		/// </summary>
		/// 
		/// <param name="mapper"></param>
		/// <param name="transl"></param>
		/// <param name="mouse"></param>
		InputPollerBase(Mapper* mapper, InputTranslaterBase<InputLibType> *transl, InputMouseBase<InputLibType> *mouse)
			: CPPThreadRunner<InputLibType>(), m(mapper), t(transl), mse(mouse)
		{
		}

		/// <summary>
		/// Explicitly deleted C++11 default constructor, intent is known there is no default constructor used here.
		/// </summary>
		InputPollerBase() = delete;

		/// <summary>
		/// Virtual destructor.
		/// </summary>
		virtual ~InputPollerBase()
		{

		}


		/// <summary>
		/// Start the workThread() polling for input (and processing via Mapper, XInputBoostMouse, XInputTranslater)
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

	};

}