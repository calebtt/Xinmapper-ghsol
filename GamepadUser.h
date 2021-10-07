#pragma once

#include "InputPoller.h"
#include "Mapper.h"
#include "XInputTranslater.h"
#include "XInputBoostMouse.h"

namespace sds
{
	/// <summary>
	/// Highest level class for usage, contains instances of InputPoller, Mapper, XInputTranslater, and XInputBoostMouse.
	/// </summary>
	class GamepadUser
	{
		/// <summary>
		/// Pointer to XInputTranslater, responsible for producing an ActionDetails from an XInputState for use
		/// by the rest of the code.
		/// </summary>
		XInputTranslater* transl;
	public:
		/// <summary>
		/// Pointer to InputPoller instance, continuously polls for input to send for processing.
		/// Check controller connected status with IsControllerConnected() and begin polling with 
		/// Start(), when finished stop with Stop()
		/// </summary>
		InputPoller *poller;
		/// <summary>
		/// Pointer to Mapper instance, remember to set the map info with the
		/// SetMapInfo() member
		/// </summary>
		Mapper *mapper;

		/// <summary>
		/// Handles achieving smooth, expected mouse movements.
		/// The class holds info on which mouse stick (if any) is to be used for controlling the mouse,
		/// the public member MouseMap enum holds this information.
		/// Includes a public member function for changing the sensitivity as well.
		/// </summary>
		XInputBoostMouse *mouse;
	public:
		GamepadUser()
		{
			mapper = new Mapper();
			transl = new XInputTranslater();
			mouse = new XInputBoostMouse();
			poller = new InputPoller(mapper,transl,mouse);
		}
		~GamepadUser()
		{
			poller->Stop();
			delete poller;
			delete mapper;
			delete transl;
			delete mouse;
		}
	};
}