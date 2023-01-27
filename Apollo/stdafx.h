// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifdef _WINDOWS
	#include "targetver.h"
	
	#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
	#include <windows.h>
#endif

#define trace(...) { printf("[Apollo][%s:%d] ", __RELFILE__, __LINE__); printf(__VA_ARGS__); printf("\n"); fflush(stdout); }

#include "apollo_state.h"
#include "gml_ext.h"
#include "gml_extm.h"
extern bool trouble;
#define trouble_check(_ret) if (trouble) return _ret;

// TODO: reference additional headers your program requires here