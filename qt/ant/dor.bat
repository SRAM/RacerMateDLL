
:: rplaced nmake with jom

if exist Desktop_Qt_5_6_0_MSVC2015_32bit goto skip
   md Desktop_Qt_5_6_0_MSVC2015_32bit
:skip

@del ..\..\racermate\release\rmg.lib
@del ..\..\racermate\cpp_tester\release\rmg.lib

cd Desktop_Qt_5_6_0_MSVC2015_32bit
qmake.exe ..\rmg.pro -r -spec win32-msvc2015 "CONFIG-=debug" "CONFIG-=qml_debug"
C:\Qt\Tools\QtCreator\bin\jom.exe  clean
C:\Qt\Tools\QtCreator\bin\jom.exe  | tee x.x
cd ..

touch ..\..\racermate\dll.cpp
touch ..\..\racermate\cpp_tester\test.cpp

