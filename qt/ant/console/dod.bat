
cd ..
@call dod.bat

cd console


:: replaced nmake with jom

if exist Desktop_Qt_5_6_0_MSVC2015_32bit goto skip
   md Desktop_Qt_5_6_0_MSVC2015_32bit
:skip


cd Desktop_Qt_5_6_0_MSVC2015_32bit
qmake.exe ..\ant_tester_console.pro -r -spec win32-msvc2015 "CONFIG+=debug" "CONFIG+=qml_debug"
C:\Qt\Tools\QtCreator\bin\jom.exe clean
C:\Qt\Tools\QtCreator\bin\jom.exe | tee x.x
copy .\debug\antcon.exe ..\
::copy ..\..\Desktop_Qt_5_6_0_MSVC2015_32bit\debug\rmant.dll ..\

cd ..



