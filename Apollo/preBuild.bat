@echo off
set dllPath=%~1
set solutionDir=%~2
set projectDir=%~3
set arch=%~4
set config=%~5

echo Running pre-build for %config%

where /q GmlCppExtFuncs
if %ERRORLEVEL% EQU 0 (
	echo Running GmlCppExtFuncs...
	GmlCppExtFuncs ^
	--prefix apollo^
	--struct 1^
	--cpp "%projectDir%autogen.cpp"^
	--gml "%solutionDir%Apollo_23/extensions/Apollo/autogen.gml"^
	--include "apollo_state.h"^
	--include "apollo_shared.h"^
	%projectDir%apollo_init.cpp^
	%projectDir%apollo_global.cpp^
	%projectDir%apollo_raw.cpp^
	%projectDir%Apollo.cpp
)