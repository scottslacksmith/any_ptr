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
#endif

#include <benchmark/benchmark.h>

#ifdef _MSC_VER
#pragma warning(pop)
#endif
