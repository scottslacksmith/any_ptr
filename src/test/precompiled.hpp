// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#ifdef __clang__
#pragma clang diagnostic ignored "-Wpragma-once-outside-header" 
#endif
#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO: reference additional headers your program requires here

#ifdef _MSC_VER
#pragma warning(push, 2)
//#pragma warning ( disable : ALL_CODE_ANALYSIS_WARNINGS 26400 26424 26461 26481 26485 26490 26491 26495 26496 26497 26499)
#endif

#include <gtest/gtest.h>

#ifdef _MSC_VER
#pragma warning(pop)
#endif