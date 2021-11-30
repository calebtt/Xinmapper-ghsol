#pragma once
#include "stdafx.h"
#include "MapFunctions.h"
#include <map>


namespace sds
{
	/// <summary>
	/// Utility class that aids in determining if an XINPUT_STATE has the token
	/// information buttons depressed. Only used in "XInputTranslater" class.
	/// </summary>
	class ButtonStateDown
	{
		sds::PlayerInfo m_localPlayer;
		using MyVariant = std::variant<std::less<>, std::greater<>>;
		using MyTuple = std::tuple<int, int, MyVariant>;
	public:
		ButtonStateDown() = default;
		ButtonStateDown(const sds::PlayerInfo &player)
		{
			m_localPlayer = player;
		}
		ButtonStateDown(const ButtonStateDown& other) = delete;
		ButtonStateDown(ButtonStateDown&& other) = delete;
		ButtonStateDown& operator=(const ButtonStateDown& other) = delete;
		ButtonStateDown& operator=(ButtonStateDown&& other) = delete;
		~ButtonStateDown() = default;
		/// <summary>
		/// Utility function that aids in determining if the button is pressed in the XINPUT_STATE
		/// </summary>
		/// <param name="state"> an XINPUT_STATE with current input from the controller</param>
		/// <param name="token"> a string specifying the button info for comparison</param>
		/// <returns>true if the button is depressed, false otherwise</returns>
		bool ButtonDown(const XINPUT_STATE& state, const std::string token) const
		{
			if(MapFunctions::IsInMap<const std::string,int>(token,sds::sdsActionDescriptors.xin_buttons))
			{
				return state.Gamepad.wButtons & sds::sdsActionDescriptors.xin_buttons.at(token);
			}
			return false;
		}
		/// <summary>
		/// Utility function that returns true if the trigger "token" is reported as depressed
		/// by the XINPUT_STATE "state" with regard to deadzone information, false otherwise.
		/// </summary>
		/// <param name="state">is an XINPUT_STATE struct with details on the current reported controller state</param>
		/// <param name="token">is a one-part token containing normally a trigger designation "LTRIGGER" or "RTRIGGER"</param>
		/// <returns>true if trigger is depressed, false otherwise</returns>
		bool TriggerDown(const XINPUT_STATE& state, const std::string token) const
		{
			if (token == sds::sdsActionDescriptors.lTrigger)
			{
				if (state.Gamepad.bLeftTrigger > m_localPlayer.left_trigger_dz)
				{
					return true;
				}
			}
			if (token == sds::sdsActionDescriptors.rTrigger)
			{
				if (state.Gamepad.bRightTrigger > m_localPlayer.right_trigger_dz)
				{
					return true;
				}
			}
			return false;
		}
		/// <summary>
		/// Utility function that returns true if the thumbstick + direction token's reported value is above the deadzone value
		/// in sdsPlayerInfo. False otherwise.
		/// </summary>
		/// <param name="state"> is an XINPUT_STATE struct with details on the current reported controller state</param>
		/// <param name="token"> is a two-part token containing normally a button and a direction for the thumbsticks,
		/// colon delimited</param>
		/// <returns>true if thumbstick+direction is pressed</returns>
		bool ThumbstickDown(const XINPUT_STATE& state, const std::string token) const
		{
			auto &&m_thumbstickMap = BuildThumbstickMap(state);
			MyTuple myTup;
			if (MapFunctions::IsInMap<std::string,MyTuple,int,MyVariant>(token, m_thumbstickMap,myTup))
			{
				const int dzVal = std::get<0>(myTup);
				const int thumbVal = std::get<1>(myTup);
				auto f = std::get<2>(myTup);
				const bool testResult = std::visit([&dzVal, &thumbVal](auto thisWillBeEitherLessOrGreater) { return thisWillBeEitherLessOrGreater(thumbVal, dzVal); }, f);
				return testResult;
			}
			return false;
		}
		/// <summary>
		/// Builds a map of string tokens to the tuple type with deadzone, current value, and functor
		///	from an XINPUT_STATE arg.
		/// </summary>
		/// <param name="state">XINPUT_STATE for processing</param>
		/// <returns>a map that maps string to tuple of deadzone,current value,functor</returns>
		[[nodiscard]] auto BuildThumbstickMap(const XINPUT_STATE &state) const -> std::map<std::string, std::tuple<int, int, std::variant<std::less<>, std::greater<>>>>
		{
			using sds::sdsActionDescriptors;
			using std::string;
			using std::map;
			using std::make_tuple;
			//map each string to each deadzone, current value, and operation "functor"
			map<string, MyTuple> someOtherMap;

			string temp = sds::sdsActionDescriptors.lThumb + sds::sdsActionDescriptors.moreInfo;
			//left thumbstick tokens
			someOtherMap[temp + sdsActionDescriptors.left] = make_tuple(-static_cast<int>(m_localPlayer.left_x_dz), state.Gamepad.sThumbLX, std::less<>());
			someOtherMap[temp + sdsActionDescriptors.right] = make_tuple(static_cast<int>(m_localPlayer.left_x_dz), state.Gamepad.sThumbLX, std::greater<>());
			someOtherMap[temp + sdsActionDescriptors.down] = make_tuple(-static_cast<int>(m_localPlayer.left_y_dz), state.Gamepad.sThumbLY, std::less<>());
			someOtherMap[temp + sdsActionDescriptors.up] = make_tuple(static_cast<int>(m_localPlayer.left_y_dz), state.Gamepad.sThumbLY, std::greater<>());

			//right thumbstick tokens
			temp = sds::sdsActionDescriptors.rThumb + sds::sdsActionDescriptors.moreInfo;
			someOtherMap[temp + sdsActionDescriptors.left] = make_tuple(-static_cast<int>(m_localPlayer.right_x_dz), state.Gamepad.sThumbRX, std::less<>());
			someOtherMap[temp + sdsActionDescriptors.right] = make_tuple(static_cast<int>(m_localPlayer.right_x_dz), state.Gamepad.sThumbRX, std::greater<>());
			someOtherMap[temp + sdsActionDescriptors.down] = make_tuple(-static_cast<int>(m_localPlayer.right_y_dz), state.Gamepad.sThumbRY, std::less<>());
			someOtherMap[temp + sdsActionDescriptors.up] = make_tuple(static_cast<int>(m_localPlayer.right_y_dz), state.Gamepad.sThumbRY, std::greater<>());
			return someOtherMap;
		}
	};
}
