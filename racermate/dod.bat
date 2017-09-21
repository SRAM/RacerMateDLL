cd ..\qt\ctsrv
@call dod.bat
cd ..\..\racermate

cd ..\qt\ant
@call dod.bat
cd ..\..\racermate

:: this done in setup.bat:
::"C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\vcvarsall.bat" x86
@del x.x
MSBuild dll_2015.sln /m /t:rebuild /p:Configuration=Debug | tee x.x
::MSBuild dll_2015.sln /p:Configuration=Debug | tee x.x


::copy debug\racermate.dll Z:\GitHub\RacerMateInc\RacerMateOne\RacerMateOne\
