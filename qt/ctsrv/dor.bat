
:: replace nmake with jom

cd ..\rmg
call dor.bat
cd ..\ctsrv

if exist Desktop_Qt_5_6_0_MSVC2015_32bit goto skip
   md Desktop_Qt_5_6_0_MSVC2015_32bit
:skip


cd Desktop_Qt_5_6_0_MSVC2015_32bit
C:\Qt\5.6\msvc2015\bin\qmake.exe ..\ctsrv.pro -r -spec win32-msvc2015 "CONFIG-=debug" "CONFIG-=qml_debug"
C:\Qt\Tools\QtCreator\bin\jom.exe clean
C:\Qt\Tools\QtCreator\bin\jom.exe | tee x.x

copy .\release\ctsrv.lib ..\..\..\racermate\release
copy .\release\ctsrv.lib ..\..\..\racermate\cpp_tester\release

:: rmg touches these
::touch ..\..\..\racermate\dll.cpp
::touch ..\..\..\racermate\cpp_tester\test.cpp

cd ..

