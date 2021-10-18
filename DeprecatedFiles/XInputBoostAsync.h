/*
This class is not used, it is included merely as a reference.
DEPRECATED: By CPPThreadRunner.h
Base class for async operations.

*/
#pragma once
#include "stdafx.h"

namespace sds
{
	/// <summary>
	/// Base class for processing input concurrently, contains pure virtual member "workThread"
	/// to be overridden by an inheriting class. The startThread() and stopThread() protected members will allow
	/// control by the inheriting class. The std::mutex stateMutex and std::shared_ptr{std::thread} thread serve
	/// as concurrency helpers with std::lock_guard{std::mutex}
	/// Pure virtual public member function "ProcessState()" enables the class to process an XINPUT_STATE
	/// directly, for the case of mouse movement where the info isn't stringified into an ActionDetails type format.
	/// </summary>
	class XInputBoostAsync
	{
		using lock = std::lock_guard<std::mutex>;

	protected:
		std::shared_ptr<std::thread> thread;
		volatile bool isThreadRunning;
		volatile bool isStopRequested;
		XINPUT_STATE local_state;
		std::mutex stateMutex;

	protected://member functions
		/// <summary>
		/// Starts running a new "workThread".
		/// </summary>
		void startThread()
		{
			if( ! this->isThreadRunning )
			{
				this->isStopRequested = false;
				this->isThreadRunning = true;
				this->thread = std::shared_ptr<std::thread>
					(new std::thread(std::bind(&XInputBoostAsync::workThread,this)));
			}
		}

		/// <summary>
		/// Non-blocking way to stop a running thread.
		/// </summary>
		void requestStop()
		{
			if( this->thread != nullptr )
			{
				//if thread not running, return
				if (!this->isThreadRunning)
				{
					return;
				}
				//else request thread stop
				else
				{
					this->isStopRequested = true;
				}
			}
		}

		/// <summary>
		/// Blocking way to stop a running thread, joins to current thread and waits.
		/// </summary>
		void stopThread()
		{
			//If there is a thread obj..
			if(this->thread != nullptr)
			{
				//if thread is not running, return
				if (!this->isThreadRunning)
				{
					return;
				}
				//else request stop and join to current thread.
				else
				{
					this->isStopRequested = true;
					this->thread->join();
					this->isThreadRunning = false;
				}
			}
		}

		/// <summary>
		/// Utility function to update the XINPUT_STATE with mutex locking thread safety.
		/// </summary>
		/// <param name="state">XINPUT_STATE obj to be copied to the internal one.</param>
		void updateState(const XINPUT_STATE &state) 
		{
			lock l1(stateMutex);
			local_state = state;
		}

		/// <summary>
		/// Returns a copy of the internal XINPUT_STATE obj with mutex locking thread safety.
		/// </summary>
		/// <returns>A copy of the internal XINPUT_STATE obj</returns>
		XINPUT_STATE getCurrentState()
		{
			lock l1(stateMutex);
			return local_state;
		}
		
		/// <summary>
		/// Pure virtual worker thread, intended to be overridden with something useful by an inheriting class.
		/// Protected visibility.
		/// </summary>
		virtual void workThread() = 0;//<<-- thread to be running.

	public://public member functions

		/// <summary>
		/// Constructor, default overridden.
		/// </summary>
		XInputBoostAsync()
		{
			isThreadRunning = false;
			isStopRequested = false;
			lock l1(stateMutex);
			memset(&local_state,0,sizeof(XINPUT_STATE));
		}
		
		/// <summary>
		/// Virtual destructor, called after the derived object's destructor is called, ensures the running
		/// thread is stopped. This function does block.
		/// </summary>
		virtual ~XInputBoostAsync()
		{
			if( this->thread != nullptr )
			{
				this->isStopRequested = true;
				this->thread->join();
			}
		}
		/// <summary>
		/// Pure virtual worker function "ProcessState", intended to be used by an inheriting class.
		/// Accepts an XINPUT_STATE obj by reference for further processing.
		/// public visibility.
		/// </summary>
		/// <param name="state">XINPUT_STATE obj for processing.</param>
		virtual void ProcessState(const XINPUT_STATE &state) = 0;

	};
}