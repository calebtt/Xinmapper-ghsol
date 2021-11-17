/*
Further design considerations would include
interpreting an ActionString,
and to allow mapping with the MapString.
Which would mean updating ActionDescriptors to include the new
tokens, and updating this class to interpret an ActionString,
and finally update GamepadUser.

Finally, the ability to modify the sensitivity function
is of utmost importance, which may require a math expression
parser.  The class currently uses the graph of [ y = (x^2) / mouseSensitivity + 1 ]
which is a parabola with the quality that at the curve's highest points
it is the nearly (or exactly) the size of the viewing window--the mouseSensitivity.
Also need sensitivity for both X and Y axis.
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
	/// This class starts a running thread that is used only to process the XINPUT_STATE structure
	/// and use those values to determine if it should move the mouse cursor, and if so how much.
	/// Another thread calls ProcessState(XINPUT_STATE) to update the internal XINPUT_STATE struct.
	/// It also has public functions for getting and setting the sensitivity.
	/// </summary>
	class XInputBoostMouse : public CPPThreadRunner<XINPUT_STATE>
	{
	public:
		enum class MouseMap : int
		{
			NEITHER_STICK,
			RIGHT_STICK,
			LEFT_STICK
		};

	private:
		SendKey keySend;
		std::atomic<MouseMap> stickMapInfo;
		std::atomic<SHORT> threadX, threadY;
		std::atomic<LONG> step;
		std::atomic<int> mouseSensitivity;
		std::atomic<bool> isDeadzoneActivated;
		std::atomic<bool> altDeadzoneConfig;
		std::atomic<float> altDeadzoneMultiplier;

		const int MOVE_THREAD_SLEEP = 10;//10 ms
		const int SENSITIVITY_MIN = 0;
		const int SENSITIVITY_MAX = 100;
	public:
		/// <summary>
		/// Ctor Initializes some configuration variables like the alternate deadzone config multiplier
		/// </summary>
		XInputBoostMouse() 
			: CPPThreadRunner(),
			mouseSensitivity(30),
			stickMapInfo(MouseMap::NEITHER_STICK),
			isDeadzoneActivated(false), altDeadzoneConfig(true), altDeadzoneMultiplier(0.5f)
		{
		}

		/// <summary>
		/// Destructor overriding the base virtual destructor, the thread should be stopped
		/// in this class's destructor, not the base destructor.
		/// </summary>
		~XInputBoostMouse() override
		{ 
			this->stopThread();
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
		/// Setter for telling the processing to use the alternate deadzone processing config.
		/// </summary>
		/// <param name="useAlt"></param>
		void SetUseAltDeadzone(bool useAlt)
		{
			altDeadzoneConfig = useAlt;
		}
		/// <summary>
		/// Getter for current status of using the alternate deadzone processing config.
		/// </summary>
		/// <returns></returns>
		bool GetUseAltDeadzone() const
		{
			return altDeadzoneConfig;
		}
		/// <summary>
		/// Sets the value of the alternate deadzone multiplier.
		/// For the behavior of the alternate deadzone configuration,
		/// the multiplier is used to lower the deadzone when one is already activated.
		/// </summary>
		/// <param name="newValue"></param>
		/// <returns></returns>
		std::string SetAltDeadzoneMultiplier(float newValue)
		{
			if (newValue > 1.0f || newValue <= 0.0f)
				return "Failed to set new deadzone value, out of range.";
			altDeadzoneMultiplier = newValue;
			return "";
		}

		/// <summary>
		/// Gets the current alternate deadzone multiplier.
		/// </summary>
		/// <returns></returns>
		float GetAltDeadzoneMultiplier() const
		{
			float r = altDeadzoneMultiplier;
			return r;
		}
		/// <summary>
		/// Setter for sensitivity value
		/// </summary>
		/// <param name="new_sens"></param>
		/// <returns> returns a std::string containing an error message
		/// if there is an error, empty string otherwise. </returns>
		std::string SetSensitivity(int new_sens)
		{
			if (new_sens <= SENSITIVITY_MIN || new_sens > SENSITIVITY_MAX)
			{
				return "Error in sds::XInputBoostMouse::SetSensitivity(), int new_sens less than or equal to 0 or greater than 100.";
			}
			mouseSensitivity = new_sens;
			return "";
		}
		/// <summary>
		/// Getter for sensitivity value
		/// </summary>
		/// <returns></returns>
		int GetSensitivity() const
		{
			return mouseSensitivity;
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
			if (stickMapInfo == MouseMap::NEITHER_STICK)
				return false;

			int DEADZONE = stickMapInfo == MouseMap::LEFT_STICK ? sds::sdsPlayerOne.left_dz : sds::sdsPlayerOne.right_dz;
			bool xMove = (x > DEADZONE || x < -DEADZONE);
			bool yMove = (y > DEADZONE || y < -DEADZONE);

			if (!xMove && !yMove)
			{
				isDeadzoneActivated = false;
				return false;
			}
			else
			{
				isDeadzoneActivated = true;
				return true;
			}
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
			int t_sens = mouseSensitivity;

			if( x > 0 )
			{
				if (this->stickMapInfo == MouseMap::LEFT_STICK)
				{
					if (this->altDeadzoneConfig && this->isDeadzoneActivated)
						x -= static_cast<LONG>(sdsPlayerOne.left_dz * altDeadzoneMultiplier);
					else
						x -= sdsPlayerOne.left_dz;

					x = NormalizeRange(0, std::numeric_limits<SHORT>::max(),
						static_cast<LONG>(t_sens), x);
				}
				else
				{
					if (this->altDeadzoneConfig && this->isDeadzoneActivated)
						x -= static_cast<LONG>(sdsPlayerOne.right_dz * altDeadzoneMultiplier);
					else
						x -= sdsPlayerOne.right_dz;
					
					x = NormalizeRange(0, std::numeric_limits<SHORT>::max(),
						static_cast<LONG>(t_sens), x);
				}
			}
			else
			{
				if (this->stickMapInfo == MouseMap::LEFT_STICK)
				{
					if (this->altDeadzoneConfig && this->isDeadzoneActivated)
						x += static_cast<LONG>(sdsPlayerOne.left_dz * altDeadzoneMultiplier);
					else
						x += sdsPlayerOne.left_dz;
					x = -NormalizeRange(0, std::numeric_limits<SHORT>::max(),
						static_cast<LONG>(t_sens), -x);
				}
				else
				{
					if (this->altDeadzoneConfig && this->isDeadzoneActivated)
						x += static_cast<LONG>(sdsPlayerOne.right_dz * altDeadzoneMultiplier);
					else
						x += sdsPlayerOne.right_dz;
					x = -NormalizeRange(0, std::numeric_limits<SHORT>::max(),
						static_cast<LONG>(t_sens), -x);
				}
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
		/// Sensitivity function is the graph of x*x/mouseSensitivity + 1
		/// returns a sensitivity normalized value; or 1 if the result is 0
		/// </summary>
		/// <param name="x">normalized value to adjust for sensitivity</param>
		/// <returns> a sensitivity normalized value or 1 if the result is 0</returns>
		size_t getFunctionalValue(size_t x) const
		{
			x = static_cast<size_t>((x * x) / mouseSensitivity + 1);
			return (x == 0) ? 1 : x;
		}

		/// <summary>
		/// Runs through the gamut of utility functions to get the number of pixels to move
		/// before moving the mouse.
		/// </summary>
		/// <param name="x"></param>
		/// <param name="y"></param>
		void doInput(LONG x, LONG y)
		{
			//NormalizeAxis will subtract (or add) the deadzone value from the numbers.
			LONG x_value = NormalizeAxis(x);
			LONG y_value = 0;

			//Have to be careful inverting signs,
			//can overflow easily!
			if( y <= std::numeric_limits<SHORT>::min() )
			{
				y = std::numeric_limits<SHORT>::min() + 1;
			}

			//here y is inverted, to invert the y axis.
			//this is because the coordinate plane for pixels starts at the top-left on windows
			y_value = NormalizeAxis(-y);

			if( x_value != 0 )
			{
				if( x_value > 0 )
				{
					x_value = getFunctionalValue(x_value);
				}
				else
				{
					x_value = -(LONG)getFunctionalValue(abs(x_value));
				}
			}

			if( y_value != 0 )
			{
				if( y_value > 0 )
				{
					y_value = getFunctionalValue(y_value);
				}
				else
				{
					y_value = -(LONG)getFunctionalValue(abs(y_value));
				}
			}

			//Finally, send the input
			keySend.SendMouseMove(x_value, y_value);
		}



	};
}