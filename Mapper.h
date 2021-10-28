#pragma once
#include "stdafx.h"
#include "Globals.h"
#include "MultiBool.h"
#include "SendKey.h"

namespace sds
{
	/// <summary>
	/// Contains the logic for determining if a key press or mouse click should occur, uses sds::SendKey keySend to send the input.
	/// Processes the ActionDetails utility class.
	/// Further design considerations may incorporate a queue for sending input, as SendInput will allow an entire array to be
	/// sent in one call.
	/// </summary>
	class Mapper
	{
		/// <summary>
		/// Utility class for processing MapInformation strings.
		/// </summary>
		struct WordData
		{
			std::string control; //LTHUMB
			std::string info; //LEFT
			std::string sim_type; //NORM
			std::string value; //'a'

			sds::MultiBool fsm;
			bool down; // is key pressed?
			WordData() : down(false) {}
		};

		SendKey keySend;
		std::vector<WordData> mapTokenInfo;
		MapInformation map;
	public:

		/// <summary>
		/// Function to process an sds::ActionDetails string created by sds::XInputTranslater
		/// </summary>
		/// <param name="details">An sds::ActionDetails containing actions to perform, translated from controller input.</param>
		void ProcessActionDetails(ActionDetails details)
		{
			std::vector<std::string> tokens;
			//Get input tokens.
			GetTokens(details,tokens);
			//Delegate processing.
			ProcessTokens(tokens);
		}

		/// <summary>
		/// Returns a copy of the local, existing MapInformation string.
		/// </summary>
		/// <returns></returns>
		MapInformation GetMapInfo() const
		{
			return map;
		}
		/// <summary>
		/// Takes a "MapInformation" string and internalizes (copies) it to adjust how controller input is mapped
		/// to keyboard and mouse input.
		/// </summary>
		/// <param name="newMap">MapInformation string containing info on how to map controller input to kbd/mouse.</param>
		/// <returns>A std::string indicating the presence of an error, and the error message.</returns>
		std::string SetMapInfo(const MapInformation &newMap)
		{
			if (newMap.size() == 0)
			{
				return "Error in sds::Mapper::SetMapInfo(), MapInformation newMap size() is 0.";
			}
			//Reset map token info.
			mapTokenInfo.clear();
			//Set MapInformation
			map = newMap;
			//Set WordData vector.
			std::string previousToken;
			std::string t;
			std::stringstream ss(newMap);
			while( ss >> t )
			{
				if (t.size() == 0)
				{
					std::string errorInfo = ("Error in sds::Mapper::SetMapInfo(), a MapInformation token could not be parsed, t.size() == 0" +
						std::string(" last token parsed was: ") + previousToken);
					return errorInfo;
				}
				WordData data;
				std::replace(t.begin(), t.end(), sds::sdsActionDescriptors.moreInfo, ' ');
				std::stringstream(t) >> data.control >> data.info >> data.sim_type >> data.value;
				mapTokenInfo.push_back(data);
				previousToken = t;
			}
			return "";
		}
	private:

		/// <summary>
		/// Process ActionDetails type string tokens into WordData internal utility data structures.
		/// The string tokens are in the form of btn / trigr / thumb : more info : input sim type : value mapped to
		/// </summary>
		/// <param name="detail">(std::vector&lt;std::string&gt;) is a ref to vector of strings containing ActionDetails style tokens</param>
		void ProcessTokens(std::vector<std::string> &detail)
		{
			using std::string;

			//set all worddata "down" fields to false, the "down" member denotes that the button is currently being pressed
			//according to the current info from the XINPUT lib, translated and sent here. A finite state machine type is used
			//to keep track of the state of the key press
			//we must do this because mapTokenInfo may be filled with states from a previous
			//run of the function. The FSM MultiBool member should be left alone.
			std::for_each(mapTokenInfo.begin(), mapTokenInfo.end(), [](WordData& d) 
				{
					d.down = false;
				});


			//This section sets a bool if the map is in the input;
			//meaning if the button is reported as being pressed from the XINPUT library
			string controlButton, buttonExtraDetail;
			//iterate the vector<string> containing map tokens
			for(auto it = detail.begin(); it != detail.end(); ++it)
			{
				//replace the "moreInfo" character (which is separating values) with whitespace in the string.
				std::replace(it->begin(), it->end(), sds::sdsActionDescriptors.moreInfo, ' ');
				//grab two tokens from the input string "detail", i.e., "LTHUMB" and "LEFT"
				std::stringstream(*it) >> controlButton >> buttonExtraDetail;

				//iterate the vector of worddata internal to this class, 
				//if controlButton matches the current "worddata" AND the extra detail is NONE or matches the current "worddata"
				//then set "down" to true
				for(auto ij = mapTokenInfo.begin(); ij != mapTokenInfo.end(); ++ij)
				{
					if (controlButton == ij->control 
						&& (buttonExtraDetail + sds::sdsActionDescriptors.none == ij->info 
							|| buttonExtraDetail == ij->info))
					{
						ij->down = true;
					}
				}
			}
			//Pass on the tokenized and processed info in the form of the vector<WordData> to the input simulation helper func
			ProcessStates(this->mapTokenInfo);
		}

		/// <summary>
		/// Use the tokenized and processed form of the info we got from XInputTranslater to simulate the proper input
		/// </summary>
		/// <param name="states">is a ref to a vector of WordData used to finally simulate the input contained within</param>
		void ProcessStates(std::vector<WordData> &states)
		{
			for(auto it = states.begin(); it != states.end(); ++it)
			{
				//Update this if more sim types are added.
				if( it->sim_type == sds::sdsActionDescriptors.norm )
					Normal(*it);
				if( it->sim_type == sds::sdsActionDescriptors.toggle )
					Toggle(*it);
				if( it->sim_type == sds::sdsActionDescriptors.rapid )
					Rapid(*it);
			}
		}

		/// <summary>
		/// Normal keypress simulation logic. The enum "MultiBool" is used to good effect for
		/// tracking the current state of the keypress logic.
		/// </summary>
		/// <param name="detail"> (WordData) is a utility structure to hold info pertaining to a key binding aka MapInformation token</param>
		void Normal(WordData &detail) 
		{
			/*
			Normal keypress logic.
			The bool down member is important.
			*/
			std::string strVal;
			if( detail.down )
			{
				if (detail.fsm.current_state == MultiBool::BUTTONSTATE::STATE_ONE)
				{
					//Check for VK.
					int keyCode = GetVkFromTokenString(detail.value);
					if( keyCode >= 0 )
					{
						keySend.Send(keyCode,true);
					}
					else
						keySend.Send(detail.value,true);
					detail.fsm.current_state = MultiBool::BUTTONSTATE::STATE_TWO;
				}
			}
			else
			{
				if( detail.fsm.current_state == MultiBool::BUTTONSTATE::STATE_TWO )
				{
					//Check for VK.
					int keyCode = GetVkFromTokenString(detail.value);
					if( keyCode >= 0 )
					{
						keySend.Send(keyCode,false);
					}
					else
						keySend.Send(detail.value,false);
					detail.fsm.ResetState();
				}
			}
		}
		/// <summary>
		/// Experimental, probably doesn't work right.
		/// </summary>
		/// <param name="detail"></param>
		void Toggle(WordData &detail) 
		{
			//Toggle keypress logic.
			std::string strVal;
			if( detail.down )
			{
				if( detail.fsm.current_state == MultiBool::BUTTONSTATE::STATE_ONE )
				{
					//Check for VK
					int keyCode = GetVkFromTokenString(detail.value);
					if(keyCode >= 0)
						keySend.Send(keyCode,true);
					else
						keySend.Send(detail.value,true);
					detail.fsm.current_state = MultiBool::BUTTONSTATE::STATE_TWO;
				}
				if( detail.fsm.current_state == MultiBool::BUTTONSTATE::STATE_THREE )
				{
					int keyCode = GetVkFromTokenString(detail.value);
					if(keyCode >= 0)
						keySend.Send(keyCode,false);
					else
						keySend.Send(detail.value,false);
					detail.fsm.current_state = MultiBool::BUTTONSTATE::STATE_FOUR;
				}
			}
			else
			{
				if( detail.fsm.current_state == MultiBool::BUTTONSTATE::STATE_TWO )
				{
					detail.fsm.current_state = MultiBool::BUTTONSTATE::STATE_THREE;
				}
				if( detail.fsm.current_state == MultiBool::BUTTONSTATE::STATE_FOUR )
				{
					detail.fsm.ResetState();
				}
			}
		}
		/// <summary>
		/// Experimental, probably doesn't work right.
		/// </summary>
		/// <param name="detail"></param>
		void Rapid(WordData &detail) 
		{
			//Rapid keypress logic.
			std::string strVal;
			if(detail.down)
			{
				int keyCode = GetVkFromTokenString(detail.value);
				if( keyCode >= 0 )
				{
					keySend.Send(keyCode,true);
					keySend.Send(keyCode,false);
				}
				else
				{
					keySend.Send(detail.value,true);
					keySend.Send(detail.value,false);
				}

			}
		}

		/// <summary>
		/// Tokenizes a string into a vector&lt;string&gt;
		/// They are in the form of btn / trigr / thumb : more info : input sim type : value mapped to
		/// with the colon ':' delimiter still included, four fields in one big token.
		/// </summary>
		/// <param name="details">is the string to tokenize</param>
		/// <param name="tokenOut">is a vector&lt;string&gt; to receive the tokens</param>
		void GetTokens(const std::string &details, std::vector<std::string> &tokenOut)
		{
			tokenOut.clear();
			std::string t;
			std::stringstream ss(details);
			while( ss >> t )
				tokenOut.push_back(t);
		}

		/// <summary>
		/// Searches the input string "std::string in" and returns the Virtual Keycode as an integer.
		/// </summary>
		/// <param name="in">std::string in is a token containing a Virtual Keycode in string form</param>
		/// <returns>Returns the virtual keycode in integer form, or -1 for error.</returns>
		int GetVkFromTokenString(std::string in)//Returns -1 if no value.
		{
			int keyCode = -1;
			if( in.find(sds::sdsActionDescriptors.vk) != std::string::npos )
			{
				std::string strVal;
				strVal = in.substr(sds::sdsActionDescriptors.vk.size());
				std::stringstream(strVal) >> keyCode;
			}
			return keyCode;
		}
	};

}