// Xinmapper_2013.cpp : Defines the entry point for the console application.
//Caleb Taylor
//last updated Oct 17th, 2021
//Keep in mind need to run the .exe in administrator mode to work with programs running in admin mode.


#include "stdafx.h"
#include <WinUser.h>
#include "GamepadUser.h"

int _tmain(int argc, _TCHAR* argv[])
{
	using namespace sds;
	auto errInfo = [](const std::string e, int retVal)
	{
		std::cout << e << std::endl;
		return retVal;
	};
	MapInformation mapInfo;
	GamepadUser gamepadUser;
	mapInfo = "LTHUMB:LEFT:NORM:a LTHUMB:RIGHT:NORM:d LTHUMB:UP:NORM:w LTHUMB:DOWN:NORM:s X:NONE:NORM:r A:NONE:NORM:VK32 Y:NONE:NORM:VK164 B:NONE:NORM:VK160";
	mapInfo += " LSHOULDER:NONE:NORM:v RSHOULDER:NONE:NORM:Q LTHUMB:NONE:NORM:c RTHUMB:NONE:NORM:e START:NONE:NORM:VK27 BACK:NONE:NORM:VK8 LTRIGGER:NONE:NORM:VK2";
	mapInfo += " RTRIGGER:NONE:NORM:VK1 DPAD:LEFT:NORM:c DPAD:DOWN:NORM:x DPAD:UP:NORM:f DPAD:RIGHT:NORM:VK1";
	
	std::string err = gamepadUser.mapper->SetMapInfo(mapInfo);
	//If the string returned has a size, it is an error message.
	if (err.size())
	{
		//Error in setting the map information.
		return errInfo("Error in setting the map information. Exiting.\n" + err, 1);
	}
	err = gamepadUser.mouse->SetSensitivity(75);
	if (err.size())
	{
		return errInfo("Error in setting the mouse sensitivity. Exiting.\n" + err, 2);
	}
	gamepadUser.mouse->EnableProcessing(MouseMap::RIGHT_STICK);
	
	std::cout << "Xbox 360 controller polling started..." << std::endl;
	std::cout << "Controller reported as: " << (gamepadUser.poller->IsControllerConnected() ? "Connected." : "Disconnected.") << std::endl;
	for( ;; )
	{
		if(!gamepadUser.poller->IsRunning() && gamepadUser.poller->IsControllerConnected() )
		{
			std::cout << "Controller reported as: " << "Connected." << std::endl;
			gamepadUser.poller->Start();
		}
		if((!gamepadUser.poller->IsControllerConnected()) && gamepadUser.poller->IsRunning())
		{
			std::cout << "Controller reported as: " << "Disconnected." << std::endl;
			gamepadUser.poller->Stop();
		}
		Sleep(10);
	}
	return 0;
}