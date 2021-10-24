/*
Base class for async operations.
*/
#pragma once
#include <thread>
#include <mutex>
#include <functional>

namespace sds
{
	/// <summary>
	/// Base class for processing input concurrently, contains pure virtual member "workThread"
	/// to be overridden by an inheriting class. The startThread() and stopThread() protected members will allow
	/// control by the inheriting class. The std::mutex stateMutex and std::shared_ptr{std::thread} thread serve
	/// as concurrency helpers with std::lock_guard{std::mutex}
	/// Instantiate with the type you would like to have mutex protected access to within a running thread.
	/// </summary>

	template <class InternalData> class CPPThreadRunner
	{
		using lock = std::lock_guard<std::mutex>;

	protected:
		std::shared_ptr<std::thread> thread;

		std::atomic<bool> isThreadRunning;
		std::atomic<bool> isStopRequested;

		InternalData local_state;
		std::mutex stateMutex;
		
		/// <summary>
		/// Pure virtual worker thread, intended to be overridden with something useful by an inheriting class.
		/// Protected visibility.
		/// </summary>
		virtual void workThread() = 0;//<<-- thread to be running.

		/// <summary>
		/// Starts running a new "workThread".
		/// </summary>
		void startThread()
		{
			if( ! this->isThreadRunning )
			{
				this->isStopRequested = false;
				this->isThreadRunning = true;
				if (this->thread == nullptr)
				{
					this->thread = std::shared_ptr<std::thread>
						(new std::thread(std::bind(&CPPThreadRunner::workThread, this)));
				}
				else
				{
					this->thread.reset();
					this->thread = std::shared_ptr<std::thread>(new std::thread(std::bind(&CPPThreadRunner::workThread, this)));
				}
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
			//Get this setting out of the way.
			this->isStopRequested = true;

			//If there is a thread obj..
			if(this->thread != nullptr)
			{
				//if it is not joinable, set to nullptr
				if (!this->thread->joinable())
				{
					this->thread = nullptr;
					this->isThreadRunning = false;
					return;
				}
				else
				{
					this->thread->join();
					this->thread = nullptr;
					this->isThreadRunning = false;
				}
			}
		}

		/// <summary>
		/// Utility function to update the InternalData with mutex locking thread safety.
		/// </summary>
		/// <param name="state">InternalData obj to be copied to the internal one.</param>
		void updateState(const InternalData &state) 
		{
			lock l1(stateMutex);
			local_state = state;
		}

		/// <summary>
		/// Returns a copy of the internal InternalData obj with mutex locking thread safety.
		/// </summary>
		/// <returns>A copy of the internal InternalData obj</returns>
		InternalData getCurrentState()
		{
			lock l1(stateMutex);
			return local_state;
		}
		
	public:
		/// <summary>
		/// Constructor, default overridden, does not initialize the internal InternalData
		/// </summary>
		CPPThreadRunner()
		{
			local_state = {};
			isThreadRunning = false;
			isStopRequested = false;
		}
		
		/// <summary>
		/// Virtual destructor, called after the derived object's destructor is called, ensures the running
		/// thread is stopped. This function does block.
		/// </summary>
		virtual ~CPPThreadRunner()
		{
			if( this->thread != nullptr )
			{
				this->isStopRequested = true;
				if (this->thread->joinable())
				{
					this->thread->join();
				}
			}
		}

	};
}