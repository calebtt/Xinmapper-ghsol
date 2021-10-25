/*
Further design considerations would include
interpreting an ActionString,
and to allow mapping with the MapString.
Which would mean updating ActionDescriptors to include the new
tokens, and updating this class to interpret an ActionString,
and finally update GamepadUser.

More importantly a better measure of sensitivity should
be used rather than an arbitrary value.  One may get the 
impression Microsoft hath done the same and used a "slider"
for their sensitivity to hide this fact.

Finally, the ability to modify the sensitivity function
is of utmost importance, which may require a math expression
parser.  The class currently uses the graph of [ y = (x^2) / SENSITIVITY ]
which is a parabola with the quality that at the curve's highest points
it is the nearly (or exactly) the size of the viewing window--the SENSITIVITY.
Also need sensitivity for both X and Y axis.
*/

/*
* 10/18/21
TODO add an option that lowers the deadzone for the perpindicular axis when the current axis
is above a certain percentage of usage, this should give more granular control when
one axis is moving at a high rate of speed.
*/

#pragma once
#include "stdafx.h"
#include "CPPThreadRunner.h"

namespace sds
{
	/// <summary>
	/// Handles achieving smooth, expected mouse movements.
	/// The class holds info on which mouse stick (if any) is to be used for controlling the mouse,
	/// the MouseMap enum holds this info.
	/// </summary>
	class XInputBoostMouse : public CPPThreadRunner<XINPUT_STATE>
	{
		//small typedef for locking the mutex
		using lock = std::lock_guard<std::mutex>;

	public:
		enum class MouseMap : int
		{
			NEITHER_STICK,
			RIGHT_STICK,
			LEFT_STICK
		};// std::atomic<MouseMap> stickMapInfo;

	private:
		std::atomic<MouseMap> stickMapInfo;
		INPUT data;
		std::atomic<SHORT> threadX, threadY;
		std::atomic<LONG> step;
		std::atomic<int> SENSITIVITY;

		static const int MOVE_THREAD_SLEEP = 10;//10 ms
	public:
		XInputBoostMouse() 
			: CPPThreadRunner(),
			SENSITIVITY(30),
			stickMapInfo(MouseMap::NEITHER_STICK)
		{
			memset((LPVOID)&data,0, sizeof(INPUT));
			data.mi.dwFlags = MOUSEEVENTF_MOVE;
		}
		~XInputBoostMouse()
		{ 
			//this->HaltOperation(); 
		}
		/// <summary>
		/// Use this function to establish one stick or the other as the one controlling the mouse movements.
		/// Set to NEITHER_STICK for no thumbstick mouse movement. Options are RIGHT_STICK, LEFT_STICK, NEITHER_STICK
		/// </summary>
		/// <param name="info"> a MouseMap enum</param>
		void EnableProcessing(MouseMap info)
		{
			stickMapInfo = info;
		}

		/// <summary>
		/// Called to check XINPUT_STATE values for mouse movement requirements.
		/// Will start the workThread running if required.
		/// </summary>
		/// <param name="state"> an XINPUT_STATE </param>
		void ProcessState(const XINPUT_STATE &state)
		{
			if(stickMapInfo == MouseMap::NEITHER_STICK)
				return;
			//Holds the reported stick values and will compare to determine if movement has occurred.
			int tsx, tsy;
		
			if(stickMapInfo == MouseMap::RIGHT_STICK)
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

			if( doesRequireMove(tsx,tsy) )
			{
				//update state.
				this->updateState(state);

				//check for thread is running.
				if( !isThreadRunning )
					this->startThread();
			}
		}
		/// <summary>
		/// Setter for sensitivity value
		/// </summary>
		/// <param name="new_sens"></param>
		/// <returns> returns a std::string containing an error message
		/// if there is an error, empty string otherwise. </returns>
		std::string SetSensitivity(int new_sens)
		{
			if (new_sens <= 0)
			{
				return "Error in sds::XInputBoostMouse::SetSensitivity(), int new_sens less than or equal to 0.";
			}
			SENSITIVITY = new_sens;
			return "";
		}
		/// <summary>
		/// Getter for sensitivity value
		/// </summary>
		/// <returns></returns>
		int GetSensitivity()
		{
			int tempSens = SENSITIVITY;
			return tempSens;
		}
	private:
		/// <summary>
		/// Determines if the X or Y values are greater than the deadzone values and would
		/// thus require movement from the mouse.
		/// </summary>
		/// <param name="x">X value</param>
		/// <param name="y">Y value</param>
		/// <returns>true if requires moving the mouse, false otherwise</returns>
		bool doesRequireMove(int x, int y)
		{
			int DEADZONE = stickMapInfo == MouseMap::LEFT_STICK ? sds::sdsPlayerOne.left_dz : sds::sdsPlayerOne.right_dz;
			return ( (x > DEADZONE
				|| x < -DEADZONE)
				|| (y > DEADZONE
				|| y < -DEADZONE) );
		}

		/// <summary>
		/// Worker thread, private visibility, gets updated data from ProcessState() function to use.
		/// Accesses the std::atomic threadX and threadY members.
		/// </summary>
		virtual void workThread()
		{
			SHORT tx, ty;
			while(!isStopRequested)//<--Danger! From the base class.
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
		LONG NormalizeAxis( LONG x )
		{
			int t_sens = SENSITIVITY;

			if( x > 0 )
			{
					x -= XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
					x = NormalizeRange( 0, SHRT_MAX,
						(LONG)t_sens, x );
			}
			else
			{
					x += XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
					x = -NormalizeRange( 0, SHRT_MAX,
						(LONG)t_sens, -x );
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
		LONG NormalizeRange( LONG begin, LONG end, LONG rp, LONG val )
		{
			step = (end-begin)/rp;
			for(LONG i = begin,j=1; i <= end; i+=step,++j)
			{
				if( val >= i && val < i+step )
				{
					if(val == end)
						return j-1;
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
			data.mi.dx = 0;
			data.mi.dy = 0;
			data.mi.dwExtraInfo = GetMessageExtraInfo();
			
			LONG x_value;
			LONG y_value;
			x_value = NormalizeAxis(x);

			//Have to be careful inverting signs,
			//can overflow easily!
			if( y <= SHRT_MIN )
			{
				y=SHRT_MIN+1;
			}
			//here y is inverted, to invert the y axis.
			y_value = NormalizeAxis(-y);
	
			if( x_value != 0 )
			{
				if( x_value > 0 )
				{
					//data.mi.dx = 1;//+t_sens;
					data.mi.dx = getFunctionalValue(x_value);
				}
				else
				{
					//data.mi.dx = -1;//-t_sens;
					data.mi.dx = -(LONG)getFunctionalValue(abs(x_value));
				}
			}

			if( y_value != 0 )
			{
				if( y_value > 0 )
				{
					//data.mi.dy = 1;//+t_sens;
					data.mi.dy = getFunctionalValue(y_value);
				}
				else
				{
					//data.mi.dy = -1;//-t_sens;
					data.mi.dy = -(LONG)getFunctionalValue(abs(y_value));
				}
			}
			x_value = abs(x_value);
			y_value = abs(y_value);

			//Finally, send the input
			SendInput(1, &data, sizeof(INPUT));
		}



	};
}