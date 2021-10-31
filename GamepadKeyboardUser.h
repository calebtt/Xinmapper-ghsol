#pragma once

#include "Mapper.h"
#include "XInputKeystrokeTranslater.h"
#include "XInputKeystrokePoller.h"
#include "XInputMouse.h"

#include "InputTranslaterBase.h"
#include "InputPollerBase.h"

namespace sds
{
	/// <summary>
	/// Highest level class for usage, contains instances of XInputKeystrokePoller, Mapper, XInputKeystrokeTranslater
	/// </summary>
	class GamepadKeyboardUser
	{
		/// <summary>
		/// Pointer to XInputTranslater, responsible for producing an ActionDetails from an XInputState for use
		/// by the rest of the code.
		/// </summary>
		XInputKeystrokeTranslater *transl;
	public:
		/// <summary>
		/// Pointer to InputPoller instance, continuously polls for input to send for processing.
		/// Check controller connected status with IsControllerConnected() and begin polling with 
		/// Start(), when finished stop with Stop()
		/// </summary>
		XInputKeystrokePoller *poller;
		/// <summary>
		/// Pointer to Mapper instance, remember to set the map info with the
		/// SetMapInfo() member
		/// </summary>
		Mapper *mapper;

	public:
		GamepadKeyboardUser()
		{
			mapper = new Mapper();
			transl = new XInputKeystrokeTranslater();
			poller = new XInputKeystrokePoller(mapper, transl);
		}
		~GamepadKeyboardUser()
		{
			poller->Stop();
			delete poller;
			delete mapper;
			delete transl;
		}
	};
}