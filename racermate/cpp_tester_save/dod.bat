:: this done in setup.bat:
::"C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\vcvarsall.bat" x86
@del x.x
::MSBuild dll_2015.sln /t:rebuild /p:Configuration=Debug | tee x.x
MSBuild cpp_tester_2015.sln /p:Configuration=Debug | tee x.x