:: this done in setup.bat:
::"C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\vcvarsall.bat" x86
::MSBuild tlib_2015.sln /t:rebuild /p:Configuration=Release
MSBuild tlib_2015.sln /m /t:rebuild /p:Configuration=Release | tee x.x
