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
	/// Base class aids in handling the achieving of smooth, expected mouse movements.
	/// The class holds info on which mouse stick (if any) is to be used for controlling the mouse,
	/// the MouseMap enum holds this info.
	/// </summary>
	template <class InputLibType>
	class InputMouseBase : public CPPThreadRunner<InputLibType>
	{

	protected:
		std::atomic<MouseMap> stickMapInfo;
		std::atomic<int> SENSITIVITY;
		//static const int MOVE_THREAD_SLEEP = 10;//10 ms
		sds::SendKey keySend;

	public:
		InputMouseBase()
			: CPPThreadRunner<InputLibType>(),
			SENSITIVITY(30),
			stickMapInfo(MouseMap::NEITHER_STICK)
		{
		}
		virtual ~InputMouseBase()
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
		virtual void ProcessState(const InputLibType &state) = 0;


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
	protected:
		/// <summary>
		/// Determines if the X or Y values are greater than the deadzone values and would
		/// thusly require movement from the mouse. Deadzone values are from sds::PlayerInfo instance sds::sdsPlayerOne
		/// </summary>
		/// <param name="x">X value</param>
		/// <param name="y">Y value</param>
		/// <returns>true if requires moving the mouse, false otherwise</returns>
		bool doesRequireMove(int x, int y)
		{
			int DEADZONE = stickMapInfo == MouseMap::LEFT_STICK ? sds::sdsPlayerOne.left_dz : sds::sdsPlayerOne.right_dz;
			return ((x > DEADZONE
				|| x < -DEADZONE)
				|| (y > DEADZONE
					|| y < -DEADZONE));
		}

		/// <summary>
		/// Pure virtual worker thread, another thread updates data via ProcessState().
		/// Accesses the std::atomic threadX and threadY members.
		/// </summary>
		virtual void workThread() = 0;

		/// <summary>
		/// Runs through the gamut of utility functions to get the number of pixels to move
		/// before moving the mouse via SendKey::SendMouseMove(int x, int y)
		/// </summary>
		/// <param name="x">value of joystick in X</param>
		/// <param name="y">value of joystick in Y</param>
		virtual void doInput(LONG x, LONG y) = 0;



	};
}