#pragma once
#include "stdafx.h"
#include "InputMouseBase.h"

namespace sds 
{
	/// <summary>
	/// Handles achieving smooth, expected mouse movements.
	/// The class holds info on which mouse stick (if any) is to be used for controlling the mouse,
	/// the MouseMap enum holds this info.
	/// </summary>
	class XInputMouse : public InputMouseBase<XINPUT_STATE>
	{
	private:
		std::atomic<SHORT> threadX, threadY;
		std::atomic<LONG> step;

		static const int MOVE_THREAD_SLEEP = 10;//10 ms
	public:
		XInputMouse()
			: InputMouseBase()
		{
		}
		~XInputMouse()
		{
			//this->HaltOperation(); 
		}
		/// <summary>
		/// Called to check XINPUT_STATE values for mouse movement requirements.
		/// Will start the workThread running if required.
		/// </summary>
		/// <param name="state"> an XINPUT_STATE </param>
		void ProcessState(const XINPUT_STATE& state)
		{
			if (stickMapInfo == MouseMap::NEITHER_STICK)
				return;
			//Holds the reported stick values and will compare to determine if movement has occurred.
			int tsx, tsy;

			if (stickMapInfo == MouseMap::RIGHT_STICK)
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

			if (doesRequireMove(tsx, tsy))
			{
				//update state.
				this->updateState(state);

				//check for thread is running.
				if (!isThreadRunning)
					this->startThread();
			}
		}
	protected:

		/// <summary>
		/// Worker thread, private visibility, gets updated data from ProcessState() function to use.
		/// Accesses the std::atomic threadX and threadY members.
		/// </summary>
		virtual void workThread()
		{
			SHORT tx, ty;
			while (!isStopRequested)//<--Danger! From the base class.
			{
				//attempt do input.
				tx = threadX;
				ty = threadY;
				doInput(tx, ty);

				Sleep(MOVE_THREAD_SLEEP);
			}

			//mark thread status as not running.
			isThreadRunning = false;
		}

		/// <summary>
		/// Normalize a thumbstick value to eventually be translated into a number
		/// of pixels to move the mouse.
		/// </summary>
		/// <param name="x"></param>
		/// <returns>An adjusted number of pixels to move, to be used with the getFunctionalValue
		/// sensitivity function.</returns>
		LONG NormalizeAxis(LONG x)
		{
			int t_sens = SENSITIVITY;

			if (x > 0)
			{
				x -= XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
				x = NormalizeRange(0, SHRT_MAX,
					(LONG)t_sens, x);
			}
			else
			{
				x += XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
				x = -NormalizeRange(0, SHRT_MAX,
					(LONG)t_sens, -x);
			}
			return x;
		}

		/// <summary>
		/// WARNING: Function only works with positive values!
		/// If you need a negative value, pass a positive value and negate the result.
		/// This function takes two range values (begin, end) and a sensitivity value (rp),
		/// it also accepts the current value that the thumbstick is reporting (val) minus the deadzone value.
		/// It returns a sensitivity "normalized" value that is eventually translated into the number of
		/// pixels to move the mouse pointer after a "functional value" is obtained via "getFunctionalValue".
		/// </summary>
		/// <param name="begin"> is the range begin</param>
		/// <param name="end"> is the range end</param>
		/// <param name="rp"> is the sensitivity value to use in the calculation</param>
		/// <param name="val"> is the thumbstick value</param>
		/// <returns> a value which becomes a number of pixels to move, based on the sensitivity</returns>
		LONG NormalizeRange(LONG begin, LONG end, LONG rp, LONG val)
		{
			step = (end - begin) / rp;
			for (LONG i = begin, j = 1; i <= end; i += step, ++j)
			{
				if (val >= i && val < i + step)
				{
					if (val == end)
						return j - 1;
					return j;
				}
			}
			return 0;
		}

		/// <summary>
		/// Sensitivity function is the graph of x*x/SENSITIVITY + 1
		/// returns a sensitivity normalized value; or 1 if the result is 0
		/// </summary>
		/// <param name="x">normalized value to adjust for sensitivity</param>
		/// <returns> a sensitivity normalized value or 1 if the result is 0</returns>
		size_t getFunctionalValue(size_t x)
		{
			//x = static_cast<size_t>(4*x / SENSITIVITY +1);
			x = static_cast<size_t>((x * x) / SENSITIVITY + 1);
			//x = (size_t)ceil(((1.0/8.0)*pow((double)x,2))/2.0);
			return (x == 0) ? 1 : x;
		}

		/// <summary>
		/// Precondition: state mutex is locked.
		/// Runs through the gamut of utility functions to get the number of pixels to move
		/// before moving the mouse.
		/// </summary>
		/// <param name="x"></param>
		/// <param name="y"></param>
		void doInput(LONG x, LONG y)
		{
			static_assert(sizeof(LONG) >= sizeof(int));
			//if the result of an "abs" value call cannot be stored in the type as a positive number
			// it results in undefined behavior.
			constexpr LONG MINI = std::numeric_limits<LONG>::min();
			if (x <= MINI)
				x = MINI + 1;
			if (y <= MINI)
				y = MINI + 1;

			//xinput/windows lib typedef types
			LONG x_value;
			LONG y_value;

			x_value = NormalizeAxis(x);
			y_value = NormalizeAxis(-y); // <-- Y axis must be inverted.


			if (x_value != 0)
			{
				if (x_value > 0)
				{
					x_value = getFunctionalValue(x_value);
				}
				else
				{
					x_value = -(LONG)getFunctionalValue(abs(x_value));
				}
			}

			if (y_value != 0)
			{
				if (y_value > 0)
				{
					y_value = getFunctionalValue(y_value);
				}
				else
				{
					y_value = -(LONG)getFunctionalValue(abs(y_value));
				}
			}
			
			//Finally, send the input
			this->keySend.SendMouseMove(x_value, y_value);

			x_value = abs(x_value);
			y_value = abs(y_value);
		}
	};



}