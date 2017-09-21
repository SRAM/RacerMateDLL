
echo ************** this is post.bat *********************
cd release

@del tester.zip
@copy cpp_tester.exe tester.exe
@copy cpp_tester.exe ..
@%HOMEDRIVE%%HOMEPATH%\utils\pkzip -add tester tester.exe
@%HOMEDRIVE%%HOMEPATH%\utils\pkzip -add tester ..\..\release\racermate.dll
@%HOMEDRIVE%%HOMEPATH%\utils\pkzip -add tester ..\test.ini
@%HOMEDRIVE%%HOMEPATH%\utils\pkzip -add tester ..\pcre3.dll
@copy tester.zip c:\users\larry\desktop\tmp


