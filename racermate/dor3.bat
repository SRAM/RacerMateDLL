
Z:\_fs\racermate>cd ..\qt\ctsrv 

Z:\_fs\qt\ctsrv>cd ..\rmg 

Z:\_fs\qt\rmg>call dor.bat 

Z:\_fs\qt\rmg>if exist Desktop_Qt_5_6_0_MSVC2015_32bit goto skip 

Z:\_fs\qt\rmg>cd Desktop_Qt_5_6_0_MSVC2015_32bit 

Z:\_fs\qt\rmg\Desktop_Qt_5_6_0_MSVC2015_32bit>qmake.exe ..\rmg.pro -r -spec win32-msvc2015 "CONFIG-=debug" "CONFIG-=qml_debug" 

Z:\_fs\qt\rmg\Desktop_Qt_5_6_0_MSVC2015_32bit>C:\Qt\Tools\QtCreator\bin\jom.exe  clean 
	C:\Qt\Tools\QtCreator\bin\jom.exe -f Makefile.Release clean
	del release\comutils.obj release\smav.obj release\twopolefilter.obj release\tmr.obj release\qtformula.obj release\comglobs.obj release\person.obj release\crf.obj release\qt_data.obj release\qt_ss.obj
	C:\Qt\Tools\QtCreator\bin\jom.exe -f Makefile.Debug clean
	del debug\comutils.obj debug\smav.obj debug\twopolefilter.obj debug\tmr.obj debug\qtformula.obj debug\comglobs.obj debug\person.obj debug\crf.obj debug\qt_data.obj debug\qt_ss.obj
	del debug\rmg.vc.pdb debug\rmg.ilk debug\rmg.idb

Z:\_fs\qt\rmg\Desktop_Qt_5_6_0_MSVC2015_32bit>C:\Qt\Tools\QtCreator\bin\jom.exe    | tee x.x 
	C:\Qt\Tools\QtCreator\bin\jom.exe -f Makefile.Release
	cl -c -nologo -Zc:wchar_t -FS -Zc:strictStrings -Zc:throwingNew -O2 -MD -GR -W3 -w34100 -w34189 -w44996 -w44456 -w44457 -w44458 -wd4577 -wd4467 -EHsc -DUNICODE -DWIN32 -DQT_NO_DEBUG -DQT_CORE_LIB -DNDEBUG -I..\..\rmg -I. -I..\..\..\common -I..\..\..\rm_common -I..\..\common -IC:\Qt\5.6\msvc2015\include -IC:\Qt\5.6\msvc2015\include\QtCore -Irelease -IC:\Qt\5.6\msvc2015\mkspecs\win32-msvc2015 -Forelease\ @C:\Users\norm\AppData\Local\Temp\comutils.obj.6976.78.jom
comutils.cpp
	cl -c -nologo -Zc:wchar_t -FS -Zc:strictStrings -Zc:throwingNew -O2 -MD -GR -W3 -w34100 -w34189 -w44996 -w44456 -w44457 -w44458 -wd4577 -wd4467 -EHsc -DUNICODE -DWIN32 -DQT_NO_DEBUG -DQT_CORE_LIB -DNDEBUG -I..\..\rmg -I. -I..\..\..\common -I..\..\..\rm_common -I..\..\common -IC:\Qt\5.6\msvc2015\include -IC:\Qt\5.6\msvc2015\include\QtCore -Irelease -IC:\Qt\5.6\msvc2015\mkspecs\win32-msvc2015 -Forelease\ @C:\Users\norm\AppData\Local\Temp\comglobs.obj.6976.860.jom
comglobs.cpp
	cl -c -nologo -Zc:wchar_t -FS -Zc:strictStrings -Zc:throwingNew -O2 -MD -GR -W3 -w34100 -w34189 -w44996 -w44456 -w44457 -w44458 -wd4577 -wd4467 -EHsc -DUNICODE -DWIN32 -DQT_NO_DEBUG -DQT_CORE_LIB -DNDEBUG -I..\..\rmg -I. -I..\..\..\common -I..\..\..\rm_common -I..\..\common -IC:\Qt\5.6\msvc2015\include -IC:\Qt\5.6\msvc2015\include\QtCore -Irelease -IC:\Qt\5.6\msvc2015\mkspecs\win32-msvc2015 -Forelease\ @C:\Users\norm\AppData\Local\Temp\tmr.obj.6976.781.jom
tmr.cpp
	cl -c -nologo -Zc:wchar_t -FS -Zc:strictStrings -Zc:throwingNew -O2 -MD -GR -W3 -w34100 -w34189 -w44996 -w44456 -w44457 -w44458 -wd4577 -wd4467 -EHsc -DUNICODE -DWIN32 -DQT_NO_DEBUG -DQT_CORE_LIB -DNDEBUG -I..\..\rmg -I. -I..\..\..\common -I..\..\..\rm_common -I..\..\common -IC:\Qt\5.6\msvc2015\include -IC:\Qt\5.6\msvc2015\include\QtCore -Irelease -IC:\Qt\5.6\msvc2015\mkspecs\win32-msvc2015 -Forelease\ @C:\Users\norm\AppData\Local\Temp\smav.obj.6976.94.jom
smav.cpp
	cl -c -nologo -Zc:wchar_t -FS -Zc:strictStrings -Zc:throwingNew -O2 -MD -GR -W3 -w34100 -w34189 -w44996 -w44456 -w44457 -w44458 -wd4577 -wd4467 -EHsc -DUNICODE -DWIN32 -DQT_NO_DEBUG -DQT_CORE_LIB -DNDEBUG -I..\..\rmg -I. -I..\..\..\common -I..\..\..\rm_common -I..\..\common -IC:\Qt\5.6\msvc2015\include -IC:\Qt\5.6\msvc2015\include\QtCore -Irelease -IC:\Qt\5.6\msvc2015\mkspecs\win32-msvc2015 -Forelease\ @C:\Users\norm\AppData\Local\Temp\twopolefilter.obj.6976.110.jom
twopolefilter.cpp
	cl -c -nologo -Zc:wchar_t -FS -Zc:strictStrings -Zc:throwingNew -O2 -MD -GR -W3 -w34100 -w34189 -w44996 -w44456 -w44457 -w44458 -wd4577 -wd4467 -EHsc -DUNICODE -DWIN32 -DQT_NO_DEBUG -DQT_CORE_LIB -DNDEBUG -I..\..\rmg -I. -I..\..\..\common -I..\..\..\rm_common -I..\..\common -IC:\Qt\5.6\msvc2015\include -IC:\Qt\5.6\msvc2015\include\QtCore -Irelease -IC:\Qt\5.6\msvc2015\mkspecs\win32-msvc2015 -Forelease\ @C:\Users\norm\AppData\Local\Temp\crf.obj.6976.2360.jom
	cl -c -nologo -Zc:wchar_t -FS -Zc:strictStrings -Zc:throwingNew -O2 -MD -GR -W3 -w34100 -w34189 -w44996 -w44456 -w44457 -w44458 -wd4577 -wd4467 -EHsc -DUNICODE -DWIN32 -DQT_NO_DEBUG -DQT_CORE_LIB -DNDEBUG -I..\..\rmg -I. -I..\..\..\common -I..\..\..\rm_common -I..\..\common -IC:\Qt\5.6\msvc2015\include -IC:\Qt\5.6\msvc2015\include\QtCore -Irelease -IC:\Qt\5.6\msvc2015\mkspecs\win32-msvc2015 -Forelease\ @C:\Users\norm\AppData\Local\Temp\qtformula.obj.6976.813.jom
qtformula.cpp
	cl -c -nologo -Zc:wchar_t -FS -Zc:strictStrings -Zc:throwingNew -O2 -MD -GR -W3 -w34100 -w34189 -w44996 -w44456 -w44457 -w44458 -wd4577 -wd4467 -EHsc -DUNICODE -DWIN32 -DQT_NO_DEBUG -DQT_CORE_LIB -DNDEBUG -I..\..\rmg -I. -I..\..\..\common -I..\..\..\rm_common -I..\..\common -IC:\Qt\5.6\msvc2015\include -IC:\Qt\5.6\msvc2015\include\QtCore -Irelease -IC:\Qt\5.6\msvc2015\mkspecs\win32-msvc2015 -Forelease\ @C:\Users\norm\AppData\Local\Temp\person.obj.6976.1735.jom
person.cpp
crf.cpp
	cl -c -nologo -Zc:wchar_t -FS -Zc:strictStrings -Zc:throwingNew -O2 -MD -GR -W3 -w34100 -w34189 -w44996 -w44456 -w44457 -w44458 -wd4577 -wd4467 -EHsc -DUNICODE -DWIN32 -DQT_NO_DEBUG -DQT_CORE_LIB -DNDEBUG -I..\..\rmg -I. -I..\..\..\common -I..\..\..\rm_common -I..\..\common -IC:\Qt\5.6\msvc2015\include -IC:\Qt\5.6\msvc2015\include\QtCore -Irelease -IC:\Qt\5.6\msvc2015\mkspecs\win32-msvc2015 -Forelease\ @C:\Users\norm\AppData\Local\Temp\qt_data.obj.6976.2406.jom
qt_data.cpp
	cl -c -nologo -Zc:wchar_t -FS -Zc:strictStrings -Zc:throwingNew -O2 -MD -GR -W3 -w34100 -w34189 -w44996 -w44456 -w44457 -w44458 -wd4577 -wd4467 -EHsc -DUNICODE -DWIN32 -DQT_NO_DEBUG -DQT_CORE_LIB -DNDEBUG -I..\..\rmg -I. -I..\..\..\common -I..\..\..\rm_common -I..\..\common -IC:\Qt\5.6\msvc2015\include -IC:\Qt\5.6\msvc2015\include\QtCore -Irelease -IC:\Qt\5.6\msvc2015\mkspecs\win32-msvc2015 -Forelease\ @C:\Users\norm\AppData\Local\Temp\qt_ss.obj.6976.3453.jom
qt_ss.cpp
	lib /NOLOGO  /OUT:release\rmg.lib @C:\Users\norm\AppData\Local\Temp\rmg.lib.6976.5188.jom

Z:\_fs\qt\rmg\Desktop_Qt_5_6_0_MSVC2015_32bit>copy .\release\rmg.lib ..\..\..\racermate\release 
        1 file(s) copied.

Z:\_fs\qt\rmg\Desktop_Qt_5_6_0_MSVC2015_32bit>copy .\release\rmg.lib ..\..\..\racermate\cpp_tester\release 
        1 file(s) copied.

Z:\_fs\qt\rmg\Desktop_Qt_5_6_0_MSVC2015_32bit>cd .. 

Z:\_fs\qt\rmg>touch ..\..\racermate\dll.cpp 

Z:\_fs\qt\rmg>touch ..\..\racermate\cpp_tester\test.cpp 

Z:\_fs\qt\rmg>cd ..\ctsrv 

Z:\_fs\qt\ctsrv>if exist Desktop_Qt_5_6_0_MSVC2015_32bit goto skip 

Z:\_fs\qt\ctsrv>cd Desktop_Qt_5_6_0_MSVC2015_32bit 

Z:\_fs\qt\ctsrv\Desktop_Qt_5_6_0_MSVC2015_32bit>C:\Qt\5.6\msvc2015\bin\qmake.exe ..\ctsrv.pro -r -spec win32-msvc2015 "CONFIG-=debug" "CONFIG-=qml_debug" 

Z:\_fs\qt\ctsrv\Desktop_Qt_5_6_0_MSVC2015_32bit>C:\Qt\Tools\QtCreator\bin\jom.exe clean 
	C:\Qt\Tools\QtCreator\bin\jom.exe -f Makefile.Release clean
	del release\moc_ctsrv.cpp release\moc_udp_client.cpp release\moc_ctsrvworker.cpp
	del release\ctsrv.obj release\udp_client.obj release\ctsrvworker.obj release\sdirs.obj release\tmr.obj release\moc_ctsrv.obj release\moc_udp_client.obj release\moc_ctsrvworker.obj
	C:\Qt\Tools\QtCreator\bin\jom.exe -f Makefile.Debug clean
	del debug\moc_ctsrv.cpp debug\moc_udp_client.cpp debug\moc_ctsrvworker.cpp
	del debug\ctsrv.obj debug\udp_client.obj debug\ctsrvworker.obj debug\sdirs.obj debug\tmr.obj debug\moc_ctsrv.obj debug\moc_udp_client.obj debug\moc_ctsrvworker.obj
	del debug\ctsrv.vc.pdb debug\ctsrv.ilk debug\ctsrv.idb

Z:\_fs\qt\ctsrv\Desktop_Qt_5_6_0_MSVC2015_32bit>C:\Qt\Tools\QtCreator\bin\jom.exe   | tee x.x 
	C:\Qt\Tools\QtCreator\bin\jom.exe -f Makefile.Release
	cl -c -nologo -Zc:wchar_t -FS -Zc:strictStrings -Zc:throwingNew -O2 -MD -GR -W3 -w34100 -w34189 -w44996 -w44456 -w44457 -w44458 -wd4577 -wd4467 -EHsc -DUNICODE -DWIN32 -DQT_NO_DEBUG -DQT_NETWORK_LIB -DQT_CORE_LIB -DNDEBUG -I..\..\ctsrv -I. -I..\..\rmg -I..\..\common -I..\..\..\common -I..\..\..\rm_common -I..\..\computrainer -IC:\Qt\5.6\msvc2015\include -IC:\Qt\5.6\msvc2015\include\QtNetwork -IC:\Qt\5.6\msvc2015\include\QtCore -Irelease -IC:\Qt\5.6\msvc2015\mkspecs\win32-msvc2015 -Forelease\ @C:\Users\norm\AppData\Local\Temp\ctsrv.obj.6428.47.jom
ctsrv.cpp
	cl -c -nologo -Zc:wchar_t -FS -Zc:strictStrings -Zc:throwingNew -O2 -MD -GR -W3 -w34100 -w34189 -w44996 -w44456 -w44457 -w44458 -wd4577 -wd4467 -EHsc -DUNICODE -DWIN32 -DQT_NO_DEBUG -DQT_NETWORK_LIB -DQT_CORE_LIB -DNDEBUG -I..\..\ctsrv -I. -I..\..\rmg -I..\..\common -I..\..\..\common -I..\..\..\rm_common -I..\..\computrainer -IC:\Qt\5.6\msvc2015\include -IC:\Qt\5.6\msvc2015\include\QtNetwork -IC:\Qt\5.6\msvc2015\include\QtCore -Irelease -IC:\Qt\5.6\msvc2015\mkspecs\win32-msvc2015 -Forelease\ @C:\Users\norm\AppData\Local\Temp\tmr.obj.6428.125.jom
tmr.cpp
	cl -c -nologo -Zc:wchar_t -FS -Zc:strictStrings -Zc:throwingNew -O2 -MD -GR -W3 -w34100 -w34189 -w44996 -w44456 -w44457 -w44458 -wd4577 -wd4467 -EHsc -DUNICODE -DWIN32 -DQT_NO_DEBUG -DQT_NETWORK_LIB -DQT_CORE_LIB -DNDEBUG -I..\..\ctsrv -I. -I..\..\rmg -I..\..\common -I..\..\..\common -I..\..\..\rm_common -I..\..\computrainer -IC:\Qt\5.6\msvc2015\include -IC:\Qt\5.6\msvc2015\include\QtNetwork -IC:\Qt\5.6\msvc2015\include\QtCore -Irelease -IC:\Qt\5.6\msvc2015\mkspecs\win32-msvc2015 -Forelease\ @C:\Users\norm\AppData\Local\Temp\udp_client.obj.6428.62.jom
udp_client.cpp
	C:\Qt\5.6\msvc2015\bin\moc.exe -DUNICODE -DWIN32 -DQT_NO_DEBUG -DQT_NETWORK_LIB -DQT_CORE_LIB -DNDEBUG -D_MSC_VER=1900 -D_WIN32 -IC:/Qt/5.6/msvc2015/mkspecs/win32-msvc2015 -IZ:/_fs/qt/ctsrv -IZ:/_fs/qt/rmg -IZ:/_fs/qt/common -IZ:/_fs/common -IZ:/_fs/rm_common -IZ:/_fs/qt/computrainer -IC:/Qt/5.6/msvc2015/include -IC:/Qt/5.6/msvc2015/include/QtNetwork -IC:/Qt/5.6/msvc2015/include/QtCore -I. ..\ctsrvworker.h -o release\moc_ctsrvworker.cpp
	cl -c -nologo -Zc:wchar_t -FS -Zc:strictStrings -Zc:throwingNew -O2 -MD -GR -W3 -w34100 -w34189 -w44996 -w44456 -w44457 -w44458 -wd4577 -wd4467 -EHsc -DUNICODE -DWIN32 -DQT_NO_DEBUG -DQT_NETWORK_LIB -DQT_CORE_LIB -DNDEBUG -I..\..\ctsrv -I. -I..\..\rmg -I..\..\common -I..\..\..\common -I..\..\..\rm_common -I..\..\computrainer -IC:\Qt\5.6\msvc2015\include -IC:\Qt\5.6\msvc2015\include\QtNetwork -IC:\Qt\5.6\msvc2015\include\QtCore -Irelease -IC:\Qt\5.6\msvc2015\mkspecs\win32-msvc2015 -Forelease\ @C:\Users\norm\AppData\Local\Temp\ctsrvworker.obj.6428.78.jom
ctsrvworker.cpp
	cl -c -nologo -Zc:wchar_t -FS -Zc:strictStrings -Zc:throwingNew -O2 -MD -GR -W3 -w34100 -w34189 -w44996 -w44456 -w44457 -w44458 -wd4577 -wd4467 -EHsc -DUNICODE -DWIN32 -DQT_NO_DEBUG -DQT_NETWORK_LIB -DQT_CORE_LIB -DNDEBUG -I..\..\ctsrv -I. -I..\..\rmg -I..\..\common -I..\..\..\common -I..\..\..\rm_common -I..\..\computrainer -IC:\Qt\5.6\msvc2015\include -IC:\Qt\5.6\msvc2015\include\QtNetwork -IC:\Qt\5.6\msvc2015\include\QtCore -Irelease -IC:\Qt\5.6\msvc2015\mkspecs\win32-msvc2015 -Forelease\ @C:\Users\norm\AppData\Local\Temp\sdirs.obj.6428.109.jom
sdirs.cpp
	C:\Qt\5.6\msvc2015\bin\moc.exe -DUNICODE -DWIN32 -DQT_NO_DEBUG -DQT_NETWORK_LIB -DQT_CORE_LIB -DNDEBUG -D_MSC_VER=1900 -D_WIN32 -IC:/Qt/5.6/msvc2015/mkspecs/win32-msvc2015 -IZ:/_fs/qt/ctsrv -IZ:/_fs/qt/rmg -IZ:/_fs/qt/common -IZ:/_fs/common -IZ:/_fs/rm_common -IZ:/_fs/qt/computrainer -IC:/Qt/5.6/msvc2015/include -IC:/Qt/5.6/msvc2015/include/QtNetwork -IC:/Qt/5.6/msvc2015/include/QtCore -I. ..\udp_client.h -o release\moc_udp_client.cpp
	C:\Qt\5.6\msvc2015\bin\moc.exe -DUNICODE -DWIN32 -DQT_NO_DEBUG -DQT_NETWORK_LIB -DQT_CORE_LIB -DNDEBUG -D_MSC_VER=1900 -D_WIN32 -IC:/Qt/5.6/msvc2015/mkspecs/win32-msvc2015 -IZ:/_fs/qt/ctsrv -IZ:/_fs/qt/rmg -IZ:/_fs/qt/common -IZ:/_fs/common -IZ:/_fs/rm_common -IZ:/_fs/qt/computrainer -IC:/Qt/5.6/msvc2015/include -IC:/Qt/5.6/msvc2015/include/QtNetwork -IC:/Qt/5.6/msvc2015/include/QtCore -I. ..\ctsrv.h -o release\moc_ctsrv.cpp
	cl -c -nologo -Zc:wchar_t -FS -Zc:strictStrings -Zc:throwingNew -O2 -MD -GR -W3 -w34100 -w34189 -w44996 -w44456 -w44457 -w44458 -wd4577 -wd4467 -EHsc -DUNICODE -DWIN32 -DQT_NO_DEBUG -DQT_NETWORK_LIB -DQT_CORE_LIB -DNDEBUG -I..\..\ctsrv -I. -I..\..\rmg -I..\..\common -I..\..\..\common -I..\..\..\rm_common -I..\..\computrainer -IC:\Qt\5.6\msvc2015\include -IC:\Qt\5.6\msvc2015\include\QtNetwork -IC:\Qt\5.6\msvc2015\include\QtCore -Irelease -IC:\Qt\5.6\msvc2015\mkspecs\win32-msvc2015 -Forelease\ @C:\Users\norm\AppData\Local\Temp\moc_ctsrv.obj.6428.3703.jom
moc_ctsrv.cpp
	cl -c -nologo -Zc:wchar_t -FS -Zc:strictStrings -Zc:throwingNew -O2 -MD -GR -W3 -w34100 -w34189 -w44996 -w44456 -w44457 -w44458 -wd4577 -wd4467 -EHsc -DUNICODE -DWIN32 -DQT_NO_DEBUG -DQT_NETWORK_LIB -DQT_CORE_LIB -DNDEBUG -I..\..\ctsrv -I. -I..\..\rmg -I..\..\common -I..\..\..\common -I..\..\..\rm_common -I..\..\computrainer -IC:\Qt\5.6\msvc2015\include -IC:\Qt\5.6\msvc2015\include\QtNetwork -IC:\Qt\5.6\msvc2015\include\QtCore -Irelease -IC:\Qt\5.6\msvc2015\mkspecs\win32-msvc2015 -Forelease\ @C:\Users\norm\AppData\Local\Temp\moc_udp_client.obj.6428.5000.jom
moc_udp_client.cpp
	cl -c -nologo -Zc:wchar_t -FS -Zc:strictStrings -Zc:throwingNew -O2 -MD -GR -W3 -w34100 -w34189 -w44996 -w44456 -w44457 -w44458 -wd4577 -wd4467 -EHsc -DUNICODE -DWIN32 -DQT_NO_DEBUG -DQT_NETWORK_LIB -DQT_CORE_LIB -DNDEBUG -I..\..\ctsrv -I. -I..\..\rmg -I..\..\common -I..\..\..\common -I..\..\..\rm_common -I..\..\computrainer -IC:\Qt\5.6\msvc2015\include -IC:\Qt\5.6\msvc2015\include\QtNetwork -IC:\Qt\5.6\msvc2015\include\QtCore -Irelease -IC:\Qt\5.6\msvc2015\mkspecs\win32-msvc2015 -Forelease\ @C:\Users\norm\AppData\Local\Temp\moc_ctsrvworker.obj.6428.5406.jom
moc_ctsrvworker.cpp
	lib /NOLOGO  /OUT:release\ctsrv.lib @C:\Users\norm\AppData\Local\Temp\ctsrv.lib.6428.7375.jom

Z:\_fs\qt\ctsrv\Desktop_Qt_5_6_0_MSVC2015_32bit>copy .\release\ctsrv.lib ..\..\..\racermate\release 
        1 file(s) copied.

Z:\_fs\qt\ctsrv\Desktop_Qt_5_6_0_MSVC2015_32bit>copy .\release\ctsrv.lib ..\..\..\racermate\cpp_tester\release 
        1 file(s) copied.

Z:\_fs\qt\ctsrv\Desktop_Qt_5_6_0_MSVC2015_32bit>cd .. 

Z:\_fs\qt\ctsrv>cd ..\..\racermate 

Z:\_fs\racermate>cd ..\qt\ant 

Z:\_fs\qt\ant>if exist Desktop_Qt_5_6_0_MSVC2015_32bit goto skip 

Z:\_fs\qt\ant>cd Desktop_Qt_5_6_0_MSVC2015_32bit 

Z:\_fs\qt\ant\Desktop_Qt_5_6_0_MSVC2015_32bit>qmake.exe ..\rmg.pro -r -spec win32-msvc2015 "CONFIG-=debug" "CONFIG-=qml_debug" 

Z:\_fs\qt\ant\Desktop_Qt_5_6_0_MSVC2015_32bit>C:\Qt\Tools\QtCreator\bin\jom.exe  clean 
	C:\Qt\Tools\QtCreator\bin\jom.exe -f Makefile.Debug clean
	del debug\moc_antdev.cpp debug\moc_antworker.cpp debug\moc_ant.cpp
	del debug\antdev.obj debug\smav.obj debug\median.obj debug\tmr.obj debug\utils.obj debug\sdirs.obj debug\antworker.obj debug\ant.obj debug\antchan.obj debug\moc_antdev.obj debug\moc_antworker.obj debug\moc_ant.obj
	C:\Qt\Tools\QtCreator\bin\jom.exe -f Makefile.Release clean
	del release\moc_antdev.cpp release\moc_antworker.cpp release\moc_ant.cpp
	del release\antdev.obj release\smav.obj release\median.obj release\tmr.obj release\utils.obj release\sdirs.obj release\antworker.obj release\ant.obj release\antchan.obj release\moc_antdev.obj release\moc_antworker.obj release\moc_ant.obj
	del debug\rmant.vc.pdb debug\rmant.ilk debug\rmant.idb
	del rmant.vc.pdb
	del rmant.ilk
	del rmant.idb

Z:\_fs\qt\ant\Desktop_Qt_5_6_0_MSVC2015_32bit>C:\Qt\Tools\QtCreator\bin\jom.exe    | tee x.x 
	C:\Qt\Tools\QtCreator\bin\jom.exe -f Makefile.Debug
	cl -c -nologo -Zc:wchar_t -FS -Zc:strictStrings -Zc:throwingNew -Zi -MDd -GR -W3 -w34100 -w34189 -w44996 -w44456 -w44457 -w44458 -wd4577 -wd4467 -EHsc /Fddebug\rmant.vc.pdb -DUNICODE -DWIN32 -D_DEBUG -DQT_QML_DEBUG -DQT_CORE_LIB -I..\..\ant -I. -I..\..\..\libusb\libusb -I..\..\..\common -I..\..\common -I..\..\ant -IC:\Qt\5.6\msvc2015\include -IC:\Qt\5.6\msvc2015\include\QtCore -Idebug -IC:\Qt\5.6\msvc2015\mkspecs\win32-msvc2015 -Fodebug\ @C:\Users\norm\AppData\Local\Temp\antdev.obj.4180.63.jom
antdev.cpp
	cl -c -nologo -Zc:wchar_t -FS -Zc:strictStrings -Zc:throwingNew -Zi -MDd -GR -W3 -w34100 -w34189 -w44996 -w44456 -w44457 -w44458 -wd4577 -wd4467 -EHsc /Fddebug\rmant.vc.pdb -DUNICODE -DWIN32 -D_DEBUG -DQT_QML_DEBUG -DQT_CORE_LIB -I..\..\ant -I. -I..\..\..\libusb\libusb -I..\..\..\common -I..\..\common -I..\..\ant -IC:\Qt\5.6\msvc2015\include -IC:\Qt\5.6\msvc2015\include\QtCore -Idebug -IC:\Qt\5.6\msvc2015\mkspecs\win32-msvc2015 -Fodebug\ @C:\Users\norm\AppData\Local\Temp\utils.obj.4180.172.jom
utils.cpp
	cl -c -nologo -Zc:wchar_t -FS -Zc:strictStrings -Zc:throwingNew -Zi -MDd -GR -W3 -w34100 -w34189 -w44996 -w44456 -w44457 -w44458 -wd4577 -wd4467 -EHsc /Fddebug\rmant.vc.pdb -DUNICODE -DWIN32 -D_DEBUG -DQT_QML_DEBUG -DQT_CORE_LIB -I..\..\ant -I. -I..\..\..\libusb\libusb -I..\..\..\common -I..\..\common -I..\..\ant -IC:\Qt\5.6\msvc2015\include -IC:\Qt\5.6\msvc2015\include\QtCore -Idebug -IC:\Qt\5.6\msvc2015\mkspecs\win32-msvc2015 -Fodebug\ @C:\Users\norm\AppData\Local\Temp\tmr.obj.4180.141.jom
tmr.cpp
	cl -c -nologo -Zc:wchar_t -FS -Zc:strictStrings -Zc:throwingNew -Zi -MDd -GR -W3 -w34100 -w34189 -w44996 -w44456 -w44457 -w44458 -wd4577 -wd4467 -EHsc /Fddebug\rmant.vc.pdb -DUNICODE -DWIN32 -D_DEBUG -DQT_QML_DEBUG -DQT_CORE_LIB -I..\..\ant -I. -I..\..\..\libusb\libusb -I..\..\..\common -I..\..\common -I..\..\ant -IC:\Qt\5.6\msvc2015\include -IC:\Qt\5.6\msvc2015\include\QtCore -Idebug -IC:\Qt\5.6\msvc2015\mkspecs\win32-msvc2015 -Fodebug\ @C:\Users\norm\AppData\Local\Temp\median.obj.4180.110.jom
median.cpp
	cl -c -nologo -Zc:wchar_t -FS -Zc:strictStrings -Zc:throwingNew -Zi -MDd -GR -W3 -w34100 -w34189 -w44996 -w44456 -w44457 -w44458 -wd4577 -wd4467 -EHsc /Fddebug\rmant.vc.pdb -DUNICODE -DWIN32 -D_DEBUG -DQT_QML_DEBUG -DQT_CORE_LIB -I..\..\ant -I. -I..\..\..\libusb\libusb -I..\..\..\common -I..\..\common -I..\..\ant -IC:\Qt\5.6\msvc2015\include -IC:\Qt\5.6\msvc2015\include\QtCore -Idebug -IC:\Qt\5.6\msvc2015\mkspecs\win32-msvc2015 -Fodebug\ @C:\Users\norm\AppData\Local\Temp\smav.obj.4180.94.jom
smav.cpp
	cl -c -nologo -Zc:wchar_t -FS -Zc:strictStrings -Zc:throwingNew -Zi -MDd -GR -W3 -w34100 -w34189 -w44996 -w44456 -w44457 -w44458 -wd4577 -wd4467 -EHsc /Fddebug\rmant.vc.pdb -DUNICODE -DWIN32 -D_DEBUG -DQT_QML_DEBUG -DQT_CORE_LIB -I..\..\ant -I. -I..\..\..\libusb\libusb -I..\..\..\common -I..\..\common -I..\..\ant -IC:\Qt\5.6\msvc2015\include -IC:\Qt\5.6\msvc2015\include\QtCore -Idebug -IC:\Qt\5.6\msvc2015\mkspecs\win32-msvc2015 -Fodebug\ @C:\Users\norm\AppData\Local\Temp\sdirs.obj.4180.203.jom
sdirs.cpp
	C:\Qt\5.6\msvc2015\bin\moc.exe -DUNICODE -DWIN32 -D_DEBUG -DQT_QML_DEBUG -DQT_CORE_LIB -D_MSC_VER=1900 -D_WIN32 -IC:/Qt/5.6/msvc2015/mkspecs/win32-msvc2015 -IZ:/_fs/qt/ant -IZ:/_fs/libusb/libusb -IZ:/_fs/common -IZ:/_fs/qt/common -IZ:/_fs/qt/ant -IC:/Qt/5.6/msvc2015/include -IC:/Qt/5.6/msvc2015/include/QtCore -I. ..\antdev.h -o debug\moc_antdev.cpp
	cl -c -nologo -Zc:wchar_t -FS -Zc:strictStrings -Zc:throwingNew -Zi -MDd -GR -W3 -w34100 -w34189 -w44996 -w44456 -w44457 -w44458 -wd4577 -wd4467 -EHsc /Fddebug\rmant.vc.pdb -DUNICODE -DWIN32 -D_DEBUG -DQT_QML_DEBUG -DQT_CORE_LIB -I..\..\ant -I. -I..\..\..\libusb\libusb -I..\..\..\common -I..\..\common -I..\..\ant -IC:\Qt\5.6\msvc2015\include -IC:\Qt\5.6\msvc2015\include\QtCore -Idebug -IC:\Qt\5.6\msvc2015\mkspecs\win32-msvc2015 -Fodebug\ @C:\Users\norm\AppData\Local\Temp\antchan.obj.4180.3422.jom
antchan.cpp
	cl -c -nologo -Zc:wchar_t -FS -Zc:strictStrings -Zc:throwingNew -Zi -MDd -GR -W3 -w34100 -w34189 -w44996 -w44456 -w44457 -w44458 -wd4577 -wd4467 -EHsc /Fddebug\rmant.vc.pdb -DUNICODE -DWIN32 -D_DEBUG -DQT_QML_DEBUG -DQT_CORE_LIB -I..\..\ant -I. -I..\..\..\libusb\libusb -I..\..\..\common -I..\..\common -I..\..\ant -IC:\Qt\5.6\msvc2015\include -IC:\Qt\5.6\msvc2015\include\QtCore -Idebug -IC:\Qt\5.6\msvc2015\mkspecs\win32-msvc2015 -Fodebug\ @C:\Users\norm\AppData\Local\Temp\ant.obj.4180.3282.jom
ant.cpp
	cl -c -nologo -Zc:wchar_t -FS -Zc:strictStrings -Zc:throwingNew -Zi -MDd -GR -W3 -w34100 -w34189 -w44996 -w44456 -w44457 -w44458 -wd4577 -wd4467 -EHsc /Fddebug\rmant.vc.pdb -DUNICODE -DWIN32 -D_DEBUG -DQT_QML_DEBUG -DQT_CORE_LIB -I..\..\ant -I. -I..\..\..\libusb\libusb -I..\..\..\common -I..\..\common -I..\..\ant -IC:\Qt\5.6\msvc2015\include -IC:\Qt\5.6\msvc2015\include\QtCore -Idebug -IC:\Qt\5.6\msvc2015\mkspecs\win32-msvc2015 -Fodebug\ @C:\Users\norm\AppData\Local\Temp\antworker.obj.4180.1844.jom
antworker.cpp
	C:\Qt\5.6\msvc2015\bin\moc.exe -DUNICODE -DWIN32 -D_DEBUG -DQT_QML_DEBUG -DQT_CORE_LIB -D_MSC_VER=1900 -D_WIN32 -IC:/Qt/5.6/msvc2015/mkspecs/win32-msvc2015 -IZ:/_fs/qt/ant -IZ:/_fs/libusb/libusb -IZ:/_fs/common -IZ:/_fs/qt/common -IZ:/_fs/qt/ant -IC:/Qt/5.6/msvc2015/include -IC:/Qt/5.6/msvc2015/include/QtCore -I. ..\antworker.h -o debug\moc_antworker.cpp
	C:\Qt\5.6\msvc2015\bin\moc.exe -DUNICODE -DWIN32 -D_DEBUG -DQT_QML_DEBUG -DQT_CORE_LIB -D_MSC_VER=1900 -D_WIN32 -IC:/Qt/5.6/msvc2015/mkspecs/win32-msvc2015 -IZ:/_fs/qt/ant -IZ:/_fs/libusb/libusb -IZ:/_fs/common -IZ:/_fs/qt/common -IZ:/_fs/qt/ant -IC:/Qt/5.6/msvc2015/include -IC:/Qt/5.6/msvc2015/include/QtCore -I. ..\ant.h -o debug\moc_ant.cpp
	cl -c -nologo -Zc:wchar_t -FS -Zc:strictStrings -Zc:throwingNew -Zi -MDd -GR -W3 -w34100 -w34189 -w44996 -w44456 -w44457 -w44458 -wd4577 -wd4467 -EHsc /Fddebug\rmant.vc.pdb -DUNICODE -DWIN32 -D_DEBUG -DQT_QML_DEBUG -DQT_CORE_LIB -I..\..\ant -I. -I..\..\..\libusb\libusb -I..\..\..\common -I..\..\common -I..\..\ant -IC:\Qt\5.6\msvc2015\include -IC:\Qt\5.6\msvc2015\include\QtCore -Idebug -IC:\Qt\5.6\msvc2015\mkspecs\win32-msvc2015 -Fodebug\ @C:\Users\norm\AppData\Local\Temp\moc_antdev.obj.4180.7407.jom
moc_antdev.cpp
	cl -c -nologo -Zc:wchar_t -FS -Zc:strictStrings -Zc:throwingNew -Zi -MDd -GR -W3 -w34100 -w34189 -w44996 -w44456 -w44457 -w44458 -wd4577 -wd4467 -EHsc /Fddebug\rmant.vc.pdb -DUNICODE -DWIN32 -D_DEBUG -DQT_QML_DEBUG -DQT_CORE_LIB -I..\..\ant -I. -I..\..\..\libusb\libusb -I..\..\..\common -I..\..\common -I..\..\ant -IC:\Qt\5.6\msvc2015\include -IC:\Qt\5.6\msvc2015\include\QtCore -Idebug -IC:\Qt\5.6\msvc2015\mkspecs\win32-msvc2015 -Fodebug\ @C:\Users\norm\AppData\Local\Temp\moc_antworker.obj.4180.7532.jom
moc_antworker.cpp
	cl -c -nologo -Zc:wchar_t -FS -Zc:strictStrings -Zc:throwingNew -Zi -MDd -GR -W3 -w34100 -w34189 -w44996 -w44456 -w44457 -w44458 -wd4577 -wd4467 -EHsc /Fddebug\rmant.vc.pdb -DUNICODE -DWIN32 -D_DEBUG -DQT_QML_DEBUG -DQT_CORE_LIB -I..\..\ant -I. -I..\..\..\libusb\libusb -I..\..\..\common -I..\..\common -I..\..\ant -IC:\Qt\5.6\msvc2015\include -IC:\Qt\5.6\msvc2015\include\QtCore -Idebug -IC:\Qt\5.6\msvc2015\mkspecs\win32-msvc2015 -Fodebug\ @C:\Users\norm\AppData\Local\Temp\moc_ant.obj.4180.7735.jom
moc_ant.cpp
	lib /NOLOGO  /OUT:debug\rmant.lib @C:\Users\norm\AppData\Local\Temp\rmant.lib.4180.10438.jom

Z:\_fs\qt\ant\Desktop_Qt_5_6_0_MSVC2015_32bit>cd .. 

Z:\_fs\qt\ant>touch ..\..\racermate\dll.cpp 

Z:\_fs\qt\ant>touch ..\..\racermate\cpp_tester\test.cpp 

Z:\_fs\qt\ant>cd ..\..\racermate 

Z:\_fs\racermate>MSBuild dll_2015.sln /m /t:rebuild /p:Configuration=Release   | tee x.x 
Microsoft (R) Build Engine version 14.0.25420.1
Copyright (C) Microsoft Corporation. All rights reserved.

Build started 9/12/2017 2:04:21 PM.
     1>Project "Z:\_fs\racermate\dll_2015.sln" on node 1 (rebuild target(s)).
     1>ValidateSolutionConfiguration:
         Building solution configuration "Release|Win32".
     1>Project "Z:\_fs\racermate\dll_2015.sln" (1) is building "Z:\_fs\racermate\dll.vcxproj.metaproj" (2) on node 1 (Rebuild target(s)).
     2>Project "Z:\_fs\racermate\dll.vcxproj.metaproj" (2) is building "Z:\_fs\common\common.vcxproj" (5) on node 2 (Rebuild target(s)).
     5>_PrepareForClean:
         Deleting file "Release\common.tlog\common.lastbuildstate".
       InitializeBuildStatus:
         Creating "Release\common.tlog\unsuccessfulbuild" because "AlwaysCreate" was specified.
       ClCompile:
         C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\CL.exe /c /Zi /nologo /W3 /WX- /O2 /Oi /Oy- /GL /D WIN32 /D NDEBUG /D _LIB /D _MBCS /Gm- /EHsc /MD /GS /Gy /fp:precise /Zc:wchar_t /Zc:forScope /Zc:inline /Fo"Release\\" /Fd"Release\common.pdb" /Gd /TP /analyze- /errorReport:queue comglobs.cpp comutils.cpp crf.cpp person.cpp smav.cpp twopolefilter.cpp
         comglobs.cpp
         comutils.cpp
         crf.cpp
         person.cpp
         smav.cpp
         twopolefilter.cpp
       Lib:
         C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\Lib.exe /OUT:"Z:\_fs\racermate\Release\common.lib" /NOLOGO /LTCG Release\comglobs.obj
         Release\comutils.obj
         Release\crf.obj
         Release\person.obj
         Release\smav.obj
         Release\twopolefilter.obj
         common.vcxproj -> Z:\_fs\racermate\Release\common.lib
       FinalizeBuildStatus:
         Deleting file "Release\common.tlog\unsuccessfulbuild".
         Touching "Release\common.tlog\common.lastbuildstate".
     5>Done Building Project "Z:\_fs\common\common.vcxproj" (Rebuild target(s)).
     2>Project "Z:\_fs\racermate\dll.vcxproj.metaproj" (2) is building "Z:\_fs\racermate\dll.vcxproj" (6) on node 2 (Rebuild target(s)).
     6>_PrepareForClean:
         Deleting file "Release\racermate.tlog\racermate.lastbuildstate".
     6>Project "Z:\_fs\racermate\dll.vcxproj" (6) is building "Z:\_fs\tinythread++\tinythread++.vcxproj" (7) on node 2 (Clean target(s)).
     7>_PrepareForClean:
         Deleting file "Release\tinythread++.tlog\tinythread++.lastbuildstate".
     1>Project "Z:\_fs\racermate\dll_2015.sln" (1) is building "Z:\_fs\racermate\..\tlib\trainer_lib.vcxproj.metaproj" (4) on node 1 (Rebuild target(s)).
     4>Project "Z:\_fs\racermate\..\tlib\trainer_lib.vcxproj.metaproj" (4) is building "Z:\_fs\tlib\trainer_lib.vcxproj" (8) on node 3 (Rebuild target(s)).
     8>_PrepareForClean:
         Deleting file "Release\tlib.tlog\tlib.lastbuildstate".
     1>Project "Z:\_fs\racermate\dll_2015.sln" (1) is building "Z:\_fs\racermate\..\glib\gen_lib.vcxproj.metaproj" (3) on node 1 (Rebuild target(s)).
     3>Project "Z:\_fs\racermate\..\glib\gen_lib.vcxproj.metaproj" (3) is building "Z:\_fs\glib\gen_lib.vcxproj" (9) on node 1 (Rebuild target(s)).
     9>_PrepareForClean:
         Deleting file "Release\glib.tlog\glib.lastbuildstate".
     7>Done Building Project "Z:\_fs\tinythread++\tinythread++.vcxproj" (Clean target(s)).
     9>InitializeBuildStatus:
         Creating "Release\glib.tlog\unsuccessfulbuild" because "AlwaysCreate" was specified.
       ClCompile:
         C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\CL.exe /c /I../freetype/include /I../freetype/include/freetype2 /I../pcre/include /I..\ant\ANT_Windows_Library_Package_v3.0\ANT_LIB\inc /I..\ant\ANT_Windows_Library_Package_v3.0\ANT_LIB\software\serial /I..\ant\ANT_Windows_Library_Package_v3.0\ANT_LIB\software\system /I..\ant\ANT_Windows_Library_Package_v3.0\ANT_LIB\software\USB\devices /I..\ant\ANT_Windows_Library_Package_v3.0\ANT_LIB\software\USB\device_handles /I..\ant\ANT_Windows_Library_Package_v3.0\ANT_LIB\software\USB /I../common /I. /Z7 /nologo /W3 /WX- /O2 /Oi /Oy- /GL /D WIN32 /D NDEBUG /D _LIB /D _MBCS /Gm- /EHsc /MD /GS /Gy /fp:precise /Zc:wchar_t /Zc:forScope /Zc:inline /Fo"Release\\" /Fd"Release\glib.pdb" /Gd /TP /analyze- /errorReport:queue ..\common\comglobs.cpp ..\common\smav.cpp ..\common\twopolefilter.cpp amplifier.cpp averagetimer.cpp axis.cpp bmp.cpp button.cpp clientsocket.cpp crc.cpp cregexp.cpp csv.cpp dir.cpp earth.cpp editbox.cpp err.cpp fasttimer.cpp fatalerror.cpp fileselector.cpp file_rotator.cpp float2d.cpp globals.cpp ini.cpp inputdlg.cpp intel.cpp listbox.cpp logger.cpp lpfilter.cpp md5.cpp modaldialog.cpp modallistbox.cpp mru.cpp png.cpp scaler.cpp oldsdirs.cpp serial.cpp signalstrengthmeter.cpp splash.cpp statictext.cpp statusdlg.cpp tga.cpp timeout.cpp tinystr.cpp tinyxml.cpp tinyxmlerror.cpp tinyxmlparser.cpp utils.cpp utilsclass.cpp wcon.cpp win_utils.cpp windower.cpp xml.cpp
     8>Project "Z:\_fs\tlib\trainer_lib.vcxproj" (8) is building "Z:\_fs\tinythread++\tinythread++.vcxproj" (7:3) on node 2 (default targets).
     7>InitializeBuildStatus:
         Creating "Release\tinythread++.tlog\unsuccessfulbuild" because "AlwaysCreate" was specified.
       ClCompile:
         C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\CL.exe /c /I../../../tinythread++ /Zi /nologo /W3 /WX- /O2 /Oi /Oy- /GL /D WIN32 /D NDEBUG /D _LIB /D _MBCS /Gm- /EHsc /MD /GS /Gy /fp:precise /Zc:wchar_t /Zc:forScope /Zc:inline /Fo"Release\\" /Fd"Release\tinythread++.pdb" /Gd /TP /analyze- /errorReport:queue tinythread.cpp
     9>ClCompile:
         comglobs.cpp
     7>ClCompile:
         tinythread.cpp
     9>ClCompile:
         smav.cpp
         twopolefilter.cpp
     7>Lib:
         C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\Lib.exe /OUT:"Z:\_fs\racermate\Release\tinythread++.lib" /NOLOGO /LTCG Release\tinythread.obj
         tinythread++.vcxproj -> Z:\_fs\racermate\Release\tinythread++.lib
       FinalizeBuildStatus:
         Deleting file "Release\tinythread++.tlog\unsuccessfulbuild".
         Touching "Release\tinythread++.tlog\tinythread++.lastbuildstate".
     7>Done Building Project "Z:\_fs\tinythread++\tinythread++.vcxproj" (default targets).
     9>ClCompile:
         amplifier.cpp
         averagetimer.cpp
         axis.cpp
         bmp.cpp
         button.cpp
         clientsocket.cpp
         crc.cpp
         cregexp.cpp
         csv.cpp
         dir.cpp
         earth.cpp
         editbox.cpp
         err.cpp
         fasttimer.cpp
         fatalerror.cpp
         fileselector.cpp
         file_rotator.cpp
         Compiling...
         float2d.cpp
         globals.cpp
         ini.cpp
         inputdlg.cpp
         intel.cpp
         listbox.cpp
         logger.cpp
         lpfilter.cpp
         md5.cpp
         modaldialog.cpp
         modallistbox.cpp
         mru.cpp
         png.cpp
         scaler.cpp
         oldsdirs.cpp
         serial.cpp
         signalstrengthmeter.cpp
         splash.cpp
         statictext.cpp
         statusdlg.cpp
         Compiling...
         tga.cpp
         timeout.cpp
         tinystr.cpp
         tinyxml.cpp
         tinyxmlerror.cpp
         tinyxmlparser.cpp
         utils.cpp
         utilsclass.cpp
         wcon.cpp
         win_utils.cpp
         windower.cpp
         xml.cpp
       Lib:
         C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\Lib.exe /OUT:"Z:\_fs\racermate\Release\glib.lib" /NOLOGO /MACHINE:X86 /LTCG Release\comglobs.obj
         Release\smav.obj
         Release\twopolefilter.obj
         Release\amplifier.obj
         Release\averagetimer.obj
         Release\axis.obj
         Release\bmp.obj
         Release\button.obj
         Release\clientsocket.obj
         Release\crc.obj
         Release\cregexp.obj
         Release\csv.obj
         Release\dir.obj
         Release\earth.obj
         Release\editbox.obj
         Release\err.obj
         Release\fasttimer.obj
         Release\fatalerror.obj
         Release\fileselector.obj
         Release\file_rotator.obj
         Release\float2d.obj
         Release\globals.obj
         Release\ini.obj
         Release\inputdlg.obj
         Release\intel.obj
         Release\listbox.obj
         Release\logger.obj
         Release\lpfilter.obj
         Release\md5.obj
         Release\modaldialog.obj
         Release\modallistbox.obj
         Release\mru.obj
         Release\png.obj
         Release\scaler.obj
         Release\oldsdirs.obj
         Release\serial.obj
         Release\signalstrengthmeter.obj
         Release\splash.obj
         Release\statictext.obj
         Release\statusdlg.obj
         Release\tga.obj
         Release\timeout.obj
         Release\tinystr.obj
         Release\tinyxml.obj
         Release\tinyxmlerror.obj
         Release\tinyxmlparser.obj
         Release\utils.obj
         Release\utilsclass.obj
         Release\wcon.obj
         Release\win_utils.obj
         Release\windower.obj
         Release\xml.obj
         gen_lib.vcxproj -> Z:\_fs\racermate\Release\glib.lib
       FinalizeBuildStatus:
         Deleting file "Release\glib.tlog\unsuccessfulbuild".
         Touching "Release\glib.tlog\glib.lastbuildstate".
     9>Done Building Project "Z:\_fs\glib\gen_lib.vcxproj" (Rebuild target(s)).
     3>Done Building Project "Z:\_fs\racermate\..\glib\gen_lib.vcxproj.metaproj" (Rebuild target(s)).
     8>InitializeBuildStatus:
         Creating "Release\tlib.tlog\unsuccessfulbuild" because "AlwaysCreate" was specified.
       ClCompile:
         C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\CL.exe /c /Ic:\Qt\5.6\msvc2015\include /IC:\Qt\5.6\msvc2015\include\QtCore /Ic:\Qt\5.6\msvc2015\include\QtNetwork /I..\freetype\include /I..\freetype\include\freetype2 /I..\pcre\include /I..\tinythread++ /I..\ant\ANT_Windows_Library_Package_v3.0\ANT_LIB\inc /I..\ant\ANT_Windows_Library_Package_v3.0\ANT_LIB\software\serial /I..\ant\ANT_Windows_Library_Package_v3.0\ANT_LIB\software\system /I..\ant\ANT_Windows_Library_Package_v3.0\ANT_LIB\software\USB\devices /I..\ant\ANT_Windows_Library_Package_v3.0\ANT_LIB\software\USB\device_handles /I..\ant\ANT_Windows_Library_Package_v3.0\ANT_LIB\software\USB /I..\common /I..\glib /I..\rm_common /I..\qt\common /I..\qt\ctsrv /I..\qt\rmg /I. /Zi /nologo /W3 /WX- /O2 /Oi /Oy- /GL /D WIN32 /D NDEBUG /D _LIB /D _MBCS /Gm- /EHsc /MD /GS /Gy /fp:precise /Zc:wchar_t /Zc:forScope /Zc:inline /Fo"Release\\" /Fd"Release\tlib.pdb" /Gd /TP /analyze- /errorReport:queue bike.cpp client.cpp client2.cpp computrainer.cpp computrainer_decoder.cpp course.cpp data.cpp datasource.cpp decoder.cpp formula.cpp heartrate.cpp monarch.cpp nvram.cpp perfdata.cpp performance.cpp performance_decoder.cpp physics.cpp rtd.cpp simdecoder.cpp simulator.cpp ss.cpp stat.cpp user.cpp velotron.cpp velotron_decoder.cpp
         bike.cpp
         client.cpp
         client2.cpp
         computrainer.cpp
         computrainer_decoder.cpp
         course.cpp
         data.cpp
         datasource.cpp
         decoder.cpp
         formula.cpp
         heartrate.cpp
         monarch.cpp
         nvram.cpp
         perfdata.cpp
         performance.cpp
         performance_decoder.cpp
         physics.cpp
         rtd.cpp
         simdecoder.cpp
         simulator.cpp
         Compiling...
         ss.cpp
         stat.cpp
         user.cpp
         velotron.cpp
         velotron_decoder.cpp
       Lib:
         C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\Lib.exe /OUT:"Z:\_fs\racermate\Release\tlib.lib" /LIBPATH:../ant_2010/ANT_Windows_Library_Package_v3.0/ANT_LIB/Debug /NOLOGO /MACHINE:X86 /LTCG Release\bike.obj
         Release\client.obj
         Release\client2.obj
         Release\computrainer.obj
         Release\computrainer_decoder.obj
         Release\course.obj
         Release\data.obj
         Release\datasource.obj
         Release\decoder.obj
         Release\formula.obj
         Release\heartrate.obj
         Release\monarch.obj
         Release\nvram.obj
         Release\perfdata.obj
         Release\performance.obj
         Release\performance_decoder.obj
         Release\physics.obj
         Release\rtd.obj
         Release\simdecoder.obj
         Release\simulator.obj
         Release\ss.obj
         Release\stat.obj
         Release\user.obj
         Release\velotron.obj
         Release\velotron_decoder.obj
         trainer_lib.vcxproj -> Z:\_fs\racermate\Release\tlib.lib
       FinalizeBuildStatus:
         Deleting file "Release\tlib.tlog\unsuccessfulbuild".
         Touching "Release\tlib.tlog\tlib.lastbuildstate".
     8>Done Building Project "Z:\_fs\tlib\trainer_lib.vcxproj" (Rebuild target(s)).
     4>Done Building Project "Z:\_fs\racermate\..\tlib\trainer_lib.vcxproj.metaproj" (Rebuild target(s)).
     6>InitializeBuildStatus:
         Creating "Release\racermate.tlog\unsuccessfulbuild" because "AlwaysCreate" was specified.
       PreBuildEvent:
         Description: ------------------- PRE.BAT ------------------------
         .\release\pre.bat
         :VCEnd
         ==================================  this is ../dll/release/pre.bat ====================================
                 1 file(s) copied.
                 1 file(s) copied.
                 1 file(s) copied.
         ==============================================================================================================
       ClCompile:
         C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\CL.exe /c /IC:\Qt\5.6\msvc2015\include /IC:\Qt\5.6\msvc2015\include\QtCore /IC:\Qt\5.6\msvc2015\include\QtNetwork /I..\libusb\libusb /I..\tinythread++ /I..\glib /I..\tlib /I..\antwrap /I..\guilib /I..\common /I..\rm_common /I..\qt\common /I..\qt\rmg /I..\qt\ctsrv /I..\qt\ant /I. /nologo /W3 /WX- /O2 /Ob1 /Oy- /D WIN32 /D NDEBUG /D _WINDOWS /D _USRDLL /D DLL_EXPORTS /D _VC80_UPGRADE=0x0600 /D _WINDLL /D _MBCS /GF /Gm- /EHsc /MD /GS /Gy /fp:precise /Zc:wchar_t /Zc:forScope /Zc:inline /Fo".\Release/" /Fd".\Release/vc140.pdb" /Gd /TP /analyze- /errorReport:queue dll.cpp dll_globals.cpp errors.cpp ev.cpp internal.cpp
         dll.cpp
         ***********************  DLL_ERGVIDEO_ACCESS  ***********************
         LOGDLL is defined !!!!!!!!!!!!!!!!!!!!!
         dll_globals.cpp
         errors.cpp
         ev.cpp
         LOGEV is defined !!!!!!!!!!!!!!!!!!!!!
         internal.cpp
         Generating Code...
       Link:
         C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\link.exe /ERRORREPORT:QUEUE /OUT:".\Release\racermate.dll" /INCREMENTAL:NO /NOLOGO /LIBPATH:C:\Qt\5.6\msvc2015\lib /LIBPATH:..\libusb\Win32\Release\lib /LIBPATH:..\freetype\lib /LIBPATH:.\release odbc32.lib odbccp32.lib comctl32.lib winmm.lib shlwapi.lib opengl32.lib glu32.lib freetype.lib ws2_32.lib qt5core.lib qt5network.lib "libusb-1.0.lib" rmg.lib ctsrv.lib rmant.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /MANIFEST /MANIFESTUAC:"level='asInvoker' uiAccess='false'" /manifest:embed /PDB:".\Release/racermate.pdb" /MAP:".\Release/racermate.map" /OPT:REF /OPT:ICF /LTCG /TLBID:1 /DYNAMICBASE:NO /IMPLIB:".\Release/racermate.lib" /MACHINE:X86 /SAFESEH /FORCE:MULTIPLE /DLL .\Release/dll.obj
         .\Release/dll_globals.obj
         .\Release/errors.obj
         .\Release/ev.obj
         .\Release/internal.obj
         Z:\_fs\racermate\Release\common.lib
         Z:\_fs\racermate\Release\glib.lib
         Z:\_fs\racermate\Release\tinythread++.lib
         Z:\_fs\racermate\Release\tlib.lib
            Creating library .\Release/racermate.lib and object .\Release/racermate.exp
     6>MSVCRT.lib(initializers.obj) : warning LNK4098: defaultlib 'libcmt.lib' conflicts with use of other libs; use /NODEFAULTLIB:library [Z:\_fs\racermate\dll.vcxproj]
         Generating code
         Finished generating code
         dll.vcxproj -> Z:\_fs\racermate\.\Release\racermate.dll
       PostBuildEvent:
         .\release\post.bat
         :VCEnd
         **********************************  START OF ../dll/release/post.bat ***************************************
                 1 file(s) copied.
                 1 file(s) copied.
                 1 file(s) copied.
                 1 file(s) copied.
                 1 file(s) copied.
         **********************************  END OF ../dll/release/post.bat ***************************************
       FinalizeBuildStatus:
         Deleting file "Release\racermate.tlog\unsuccessfulbuild".
         Touching "Release\racermate.tlog\racermate.lastbuildstate".
     6>Done Building Project "Z:\_fs\racermate\dll.vcxproj" (Rebuild target(s)).
     2>Done Building Project "Z:\_fs\racermate\dll.vcxproj.metaproj" (Rebuild target(s)).
     1>Done Building Project "Z:\_fs\racermate\dll_2015.sln" (rebuild target(s)).

Build succeeded.

       "Z:\_fs\racermate\dll_2015.sln" (rebuild target) (1) ->
       "Z:\_fs\racermate\dll.vcxproj.metaproj" (Rebuild target) (2) ->
       "Z:\_fs\racermate\dll.vcxproj" (Rebuild target) (6) ->
       (Link target) -> 
         MSVCRT.lib(initializers.obj) : warning LNK4098: defaultlib 'libcmt.lib' conflicts with use of other libs; use /NODEFAULTLIB:library [Z:\_fs\racermate\dll.vcxproj]

    1 Warning(s)
    0 Error(s)

Time Elapsed 00:01:55.93

Z:\_fs\racermate>copy release\racermate.dll Z:\GitHub\RacerMateInc\RacerMateOne\RacerMateOne\ 
        1 file(s) copied.
