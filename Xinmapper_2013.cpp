// Xinmapper_2013.cpp : Defines the entry point for the console application.
//Caleb Taylor
//last updated Oct 6th, 2021


#include "stdafx.h"
#include <WinUser.h>
#include "GamepadUser.h"

int _tmain(int argc, _TCHAR* argv[])
{
	using namespace sds;
	MapInformation mapInfo;
	GamepadUser gamepadUser;
	mapInfo = "LTHUMB:LEFT:NORM:a LTHUMB:RIGHT:NORM:d LTHUMB:UP:NORM:w LTHUMB:DOWN:NORM:s X:NONE:NORM:r A:NONE:NORM:VK32 Y:NONE:NORM:VK164 B:NONE:NORM:VK160";
	mapInfo += " LSHOULDER:NONE:NORM:v RSHOULDER:NONE:NORM:Q LTHUMB:NONE:NORM:c RTHUMB:NONE:NORM:e START:NONE:NORM:VK27 BACK:NONE:NORM:VK8 LTRIGGER:NONE:NORM:VK2";
	mapInfo += " RTRIGGER:NONE:NORM:VK1 DPAD:LEFT:NORM:c DPAD:DOWN:NORM:x DPAD:UP:NORM:f DPAD:RIGHT:RAPID:VK1";
	gamepadUser.mapper->SetMapInfo(mapInfo);
	gamepadUser.mouse->SetSensitivity(35);
	gamepadUser.mouse->EnableProcessing(XInputBoostMouse::RIGHT_STICK);
	
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