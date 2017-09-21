cd ..\qt\ctsrv
@call dor.bat
cd ..\..\racermate

cd ..\qt\ant
@call dor.bat
cd ..\..\racermate

:: this done in setup.bat:
::"C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\vcvarsall.bat" x86
@del x.x
@del release\racermate.dll
::MSBuild tlib_2015.sln /t:rebuild /p:Configuration=Release
MSBuild dll_2015.sln /m /t:rebuild /p:Configuration=Release | tee x.x

::touch ..\rm2\racermateone\rm1.cs

::copy release\racermate.dll Z:\GitHub\RacerMateInc\RacerMateOne\RacerMateOne\
