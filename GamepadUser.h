#pragma once

#include "InputPoller.h"
#include "Mapper.h"
#include "XInputTranslater.h"
#include "XInputBoostMouse.h"

namespace sds
{
	/// <summary>
	/// Highest level class for usage, contains instances of InputPoller, Mapper, XInputTranslater, and XInputBoostMouse.
	///	Note the order in which the data members are declared in the class definition is the order in which they are
	///	initialized in the constructor initializer list. Regardless of the order of the initializers!
	/// </summary>
	class GamepadUser
	{
	public:
		/// <summary>
		/// Pointer to Mapper instance, remember to set the map info with the
		/// SetMapInfo() member
		/// </summary>
		Mapper mapper;
	private:
		/// <summary>
		/// Pointer to XInputTranslater, responsible for producing an ActionDetails from an XInputState for use
		/// by the rest of the code.
		/// </summary>
		XInputTranslater transl;
	public:
		/// <summary>
		/// Handles achieving smooth, expected mouse movements.
		/// The class holds info on which mouse stick (if any) is to be used for controlling the mouse,
		/// the public member MouseMap enum holds this information.
		/// Includes a public member function for changing the sensitivity as well.
		/// </summary>
		XInputBoostMouse mouse;
		/// <summary>
		/// Pointer to InputPoller instance, continuously polls for input to send for processing.
		/// Check controller connected status with IsControllerConnected() and begin polling with 
		/// Start(), when finished stop with Stop()
		/// </summary>
		InputPoller poller;
	public:
		GamepadUser() : poller(mapper,transl,mouse)	{ }
		GamepadUser(const sds::PlayerInfo &player) : transl(player), mouse(player), poller(mapper,transl,mouse) { }
		GamepadUser(const GamepadUser& other) = delete;
		GamepadUser(GamepadUser&& other) = delete;
		GamepadUser& operator=(const GamepadUser& other) = delete;
		GamepadUser& operator=(GamepadUser&& other) = delete;
		~GamepadUser()
		{
			poller.Stop();
			mouse.EnableProcessing(MouseMap::NEITHER_STICK);
		}
	};
}