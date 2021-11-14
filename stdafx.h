// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif


#include <windows.h>
#include <Xinput.h>
//#include <process.h>
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
#include <chrono>



// Include some commonly used global stuff.
#include "ActionDescriptors.h"
#include "Globals.h"
#include "MouseMap.h"
#include "PlayerInfo.h"
//#include "SendKey.h"