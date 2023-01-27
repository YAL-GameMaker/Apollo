@echo off

if not exist "Apollo-for-GMS2.3+\extensions" mkdir "Apollo-for-GMS2.3+\extensions"
if not exist "Apollo-for-GMS2.3+\datafiles" mkdir "Apollo-for-GMS2.3+\datafiles"
cmd /C copyre ..\Apollo_23\extensions\Apollo Apollo-for-GMS2.3+\extensions\Apollo
cmd /C copyre ..\Apollo_23\datafiles\Apollo.html Apollo-for-GMS2.3+\datafiles\Apollo.html
cd Apollo-for-GMS2.3+
cmd /C 7z a Apollo-for-GMS2.3+.zip *
move /Y Apollo-for-GMS2.3+.zip ../Apollo-for-GM2022+.yymps
cd ..

pause