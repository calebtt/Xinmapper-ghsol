/*
/////////////////////////////////////////////////////////////////
//	TODO.													/////
/////////////////////////////////////////////////////////////////
--XInputMouse.h--
This class is not used, it is included merely as a reference.

*/

#pragma once
#include "stdafx.h"

namespace sds
{
	/// <summary>
	/// This class is not used at all, it is included merely as a reference.
	/// It is probably erroneous as well.
	/// </summary>
	class XInputMouse
	{
		typedef void (XInputMouse::*ThreadFuncType)(void);
		static const int VALUE_MAX = 32767;
		static const int VALUE_MIN = -32767;
	public: XINPUT_STATE localState;
		INPUT posData, negData;
		INPUT mouseInput;
		volatile long stopRequested;
		volatile long xThreadRunning;
		volatile long yThreadRunning;
		volatile long running;
		bool isLeftStick;
		PlayerInfo *player;
		HANDLE hXThread, hYThread;
		HANDLE hStateMutex;
		struct FuncInfo
		{
			ThreadFuncType function;
			XInputMouse *instance;
			FuncInfo(XInputMouse *inst, ThreadFuncType fun) : instance(inst), function(fun) {}
			FuncInfo() : function(NULL), instance(NULL) {}
		}fx, fy;
	public:
		XInputMouse(PlayerInfo *p, bool leftStick = false) 
			: stopRequested(FALSE),
			  xThreadRunning(FALSE),
			  yThreadRunning(FALSE),
			  player(p),
			  isLeftStick(leftStick),
			  hXThread(0),
			  hYThread(0),
			  hStateMutex(0)
		{
			assert(player != NULL);
			memset((LPVOID)&localState, 0, sizeof(XINPUT_STATE));
			memset((LPVOID)&posData,0, sizeof(INPUT));
			memset((LPVOID)&negData,0, sizeof(INPUT));
			memset(&mouseInput,0,sizeof(INPUT));
			negData.type = posData.type = INPUT_MOUSE;
			mouseInput.mi.dwFlags = negData.mi.dwFlags = posData.mi.dwFlags = MOUSEEVENTF_MOVE;
			posData.mi.dx = 1;
			posData.mi.dy = 1;
			negData.mi.dx = -1;
			negData.mi.dy = -1;
			hStateMutex = ::CreateMutexA(NULL,FALSE,"XInmapper Mouse State Mutex");
			assert(hStateMutex != NULL);
		}
		~XInputMouse()
		{
			//Stop Threads.
			CloseHandle(hStateMutex);
		}
		void ProcessState(const XINPUT_STATE &state)
		{
			//Update state.
			UpdateState(state);
			//Make sure threads are running.
			//long x =
			//	::_InterlockedCompareExchange(&xThreadRunning,FALSE,FALSE);
			//long y =
			//	::_InterlockedCompareExchange(&yThreadRunning,FALSE,FALSE);
			//if( ! (x && y) )
			//{
			//	//Ensure not running.
			//	::_InterlockedExchange(&stopRequested,TRUE);
			//	if(hXThread != NULL)
			//		::WaitForSingleObject(hXThread,INFINITE);
			//	if(hYThread != NULL)
			//		::WaitForSingleObject(hYThread,INFINITE);

			//}
		}
	private:
		void StartThreads()
		{
			::_InterlockedExchange(&stopRequested,FALSE);
			::_InterlockedExchange(&xThreadRunning,TRUE);
			::_InterlockedExchange(&yThreadRunning,TRUE);
			//Create Threads.
			_beginthread(&XInputMouse::ThreadFunc,0,(LPVOID)&fx);
			_beginthread(&XInputMouse::ThreadFunc,0,LPVOID(&fy));
		}
		void StopThreads()
		{
			::_InterlockedExchange(&stopRequested,TRUE);
			::WaitForSingleObject(hXThread,INFINITE);
			::WaitForSingleObject(hYThread,INFINITE);
		}
		void UpdateState(XINPUT_STATE state)
		{
			//Acquire lock.
			DWORD result = ::WaitForSingleObject(hStateMutex,INFINITE);
			if(result != WAIT_OBJECT_0)
				return;
			//Update state.
			localState = state;
			::ReleaseMutex(hStateMutex);
		}
		/*
		value: an integer value to be converted
		to it's absolute value.
		Returns: Absolute value trimmed
		to expected maximum or minimum
		*/
		int Absolute(int value)
		{
			if( value <= VALUE_MIN )
			{
				value = VALUE_MAX;
			}
			else if( value < 0 )
				value = -value;
			else if( value > VALUE_MAX )
				value = VALUE_MAX;
			return value;
		}
		/*
		value: X or Y value from XINPUT_STATE (thumbstick)
		Returns: true if value is greater than the deadzone.
		*/
		bool RequiresMove(int value, bool leftStick)
		{
			value = Absolute(value);
			return value >= (leftStick ? player->left_dz : player->right_dz);
		}

		/*
		value: an X or Y value from an XINPUT_STATE (for a thumbstick)
		xAxis: specifies if X or Y axis.
		Returns: The PPS for the current value, based on the
		maximum in PlayerInfo "player".
		*/
		double GetPixelsPerSecond(int value, bool xAxis)
		{
			//Get positive.
			value = Absolute(value);
			//Convert to percent.
			double tempValue = value / VALUE_MAX;
			//Multiply by max pps.
			return tempValue * (xAxis ? player->x_mouse_pps : player->y_mouse_pps);
		}
		/*
		pps: pixels per second time for the current axis value.
		Returns: Time per pixel in MS.
		*/
		double GetOnePixelTime(double pps)
		{
			//assert( pps != 0.0 );
			if( pps == 0.0 )
				return 0.0;
			//pps pixels in one second (1000 ms).
			return 1000.0/pps;
		}
		/*
		isXAxis: for x axis ?
		isPositive: for positive direction ?
		Returns: 1 if successful, 0 if failure.
		*/
		int SendMove(bool isXAxis, bool isPositive )
		{
			memset(&mouseInput,0,sizeof(INPUT));
			mouseInput.mi.dwFlags = MOUSEEVENTF_MOVE;
			int value = isPositive ? 1 : -1;
			isXAxis ? mouseInput.mi.dx = value : mouseInput.mi.dy = value;
			return SendInput(1,&mouseInput,sizeof(INPUT));
		}
	public:
		void WorkerFunction()
		{
			int xValue = 0, yValue = 0;
			double pixelTime = 0.0;
			bool sendX, sendY;
			//do
			{
				//::WaitForSingleObject(hStateMutex,INFINITE);
				xValue = isLeftStick ? localState.Gamepad.sThumbLX : localState.Gamepad.sThumbRX;
				yValue = isLeftStick ? localState.Gamepad.sThumbLY : localState.Gamepad.sThumbRY;
				if(sendX = RequiresMove(xValue,isLeftStick) )
				{
					pixelTime = GetPixelsPerSecond(xValue,true);
					pixelTime = GetOnePixelTime(pixelTime);
					SendMove(true,xValue > 0);
					if(pixelTime >= 1.0)
						Sleep((DWORD)pixelTime);
				}
				if(sendY = RequiresMove(yValue,isLeftStick) )
				{
					pixelTime = GetPixelsPerSecond(yValue,false);
					pixelTime = GetOnePixelTime(pixelTime);
					SendMove(false,yValue > 0);
					if(pixelTime >= 1.0)
						Sleep((DWORD)pixelTime);
				}
				if( ! (sendX && sendY) )
					Sleep(25);//<-- happens if no input sent.

				//::ReleaseMutex(hStateMutex);
			}//while( ! ::_InterlockedCompareExchange(&stopRequested,FALSE,FALSE) );
			//::_InterlockedExchange(&running,FALSE);
		}
		static void ThreadFunc(void *funcPtr)
		{
			FuncInfo *info = static_cast<FuncInfo*>(funcPtr);
			//(info->instance)->(info->function());
		}
	};
}