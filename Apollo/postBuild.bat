@echo off
set dllPath=%~1
set solutionDir=%~2
set projectDir=%~3
set arch=%~4
set config=%~5

echo Running post-build for %config%

set extName=Apollo
set dllName=Apollo
set gmlDir23=%solutionDir%Apollo_23
set ext23=%gmlDir23%\extensions\%extName%
set dllRel=%dllName%.dll
set cppRel=%dllName%.cpp
set cppPath=%ext23%\%cppRel%
set gmlPath=%ext23%\*.gml
set docName=%extName%.html
set docPath=%solutionDir%export\%docName%

echo Copying documentation...
copy /Y %docPath% %gmlDir23%\datafiles\%docName%

echo Combining the source files...
type "%projectDir%*.h" "%projectDir%*.cpp" >"%cppPath%" 2>nul
	
echo Running GmxGen...
	
gmxgen "%ext23%\%extName%.yy" ^
--copy "%dllPath%" "%dllRel%:%arch%"