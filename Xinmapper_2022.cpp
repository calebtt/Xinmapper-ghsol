// Xinmapper_2022.cpp : Defines the entry point for the console application.
//Caleb Taylor
//Keep in mind need to run the .exe in administrator mode to work with programs running in admin mode.
#include "stdafx.h"
#include "GamepadUser.h"

int _tmain(int argc, _TCHAR* argv[])
{
	using namespace sds;
	auto errInfo = [](const std::string e, const int retVal)
	{
		std::cerr << e << std::endl;
		return retVal;
	};
	MapInformation mapInfo;
	GamepadUser gamepadUser;
	mapInfo = "LTHUMB:LEFT:NORM:a LTHUMB:RIGHT:NORM:d LTHUMB:UP:NORM:w LTHUMB:DOWN:NORM:s X:NONE:NORM:r A:NONE:NORM:VK32 Y:NONE:NORM:VK164 B:NONE:NORM:VK160";
	mapInfo += " LSHOULDER:NONE:NORM:v RSHOULDER:NONE:NORM:Q LTHUMB:NONE:NORM:c RTHUMB:NONE:NORM:e START:NONE:NORM:VK27 BACK:NONE:NORM:VK8 LTRIGGER:NONE:NORM:VK2";
	mapInfo += " RTRIGGER:NONE:NORM:VK1 DPAD:UP:NORM:VK33 DPAD:LEFT:NORM:VK37 DPAD:DOWN:NORM:VK32 DPAD:RIGHT:NORM:VK39";
	std::string err = gamepadUser.mapper.SetMapInfo(mapInfo);
	//If the string returned has a size, it is an error message.
	if (!err.empty())
	{
		//Error in setting the map information.
		return errInfo("Error in setting the map information. Exiting.\n" + err, 1);
	}
	err = gamepadUser.mouse.SetSensitivity(85);
	if (!err.empty())
	{
		return errInfo("Error in setting the mouse sensitivity. Exiting.\n" + err, 2);
	}
	gamepadUser.mouse.EnableProcessing(MouseMap::RIGHT_STICK);
	std::cout << "Xbox 360 controller polling started..." << std::endl;
	std::cout << "Controller reported as: " << (gamepadUser.poller.IsControllerConnected() ? "Connected." : "Disconnected.") << std::endl;
	for( ;; )
	{
		if(!gamepadUser.poller.IsRunning() && gamepadUser.poller.IsControllerConnected() )
		{
			std::cout << "Controller reported as: " << "Connected." << std::endl;
			gamepadUser.poller.Start();
		}
		if((!gamepadUser.poller.IsControllerConnected()) && gamepadUser.poller.IsRunning())
		{
			std::cout << "Controller reported as: " << "Disconnected." << std::endl;
			gamepadUser.poller.Stop();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	return 0;
}