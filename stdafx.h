// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#define NOMINMAX

#include "targetver.h"
#include <windows.h>
#include <Xinput.h>
#include <process.h>
#include <tchar.h>
//#include <intrin.h>

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <map>
#include <utility>
#include <limits>

#include <cstdio>
#include <cmath>
#include <cctype>
#include <cassert>

#include <thread>
#include <memory>
#include <mutex>
#include <functional>
#include <tuple>
#include <locale>
#include <format>

/// <summary>
/// ActionDetails is a std::string specifically used to transmit state information
/// about input from the controller to the various classes that will process the information.
/// </summary>
typedef std::string ActionDetails;

/// <summary>
/// Created with input from the user (of the classes), it will likely
/// be a plain text string describing how to respond to an ActionDetails
/// 
/// The string can be tokenized into "words" separated by spaces,
/// each "word" can be further subdivided into four parts.
/// 
/// <list type="bullet">
/// <listheader><term>ex.</term></listheader>
/// <item>
/// LTHUMB:LEFT:NORM:a 
/// </item>
/// <item>
/// LTHUMB:RIGHT:NORM:d
/// </item>
/// <item>
/// LTHUMB:UP:NORM:w
/// </item>
/// <item>
/// LTHUMB:DOWN:NORM:s
/// </item>
/// </list>
/// <br/>
/// They are of the form: {btn/trigr/thumb}:{moreinfo}:{input sim type}:{value mapped to}
/// </summary>
typedef std::string MapInformation;

// Include some commonly used global stuff.
#include "Globals.h"