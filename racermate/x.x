Microsoft (R) Build Engine version 14.0.25420.1
Copyright (C) Microsoft Corporation. All rights reserved.

Build started 10/12/2017 4:08:53 PM.
     1>Project "Z:\rm6dll\racermate\dll_2015.sln" on node 1 (rebuild target(s)).
     1>ValidateSolutionConfiguration:
         Building solution configuration "Release|Win32".
     1>Project "Z:\rm6dll\racermate\dll_2015.sln" (1) is building "Z:\rm6dll\racermate\dll.vcxproj.metaproj" (2) on node 1 (Rebuild target(s)).
     2>Project "Z:\rm6dll\racermate\dll.vcxproj.metaproj" (2) is building "Z:\rm6dll\common\common.vcxproj" (5) on node 2 (Rebuild target(s)).
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
         C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\Lib.exe /OUT:"Z:\rm6dll\racermate\Release\common.lib" /NOLOGO /LTCG Release\comglobs.obj
         Release\comutils.obj
         Release\crf.obj
         Release\person.obj
         Release\smav.obj
         Release\twopolefilter.obj
         common.vcxproj -> Z:\rm6dll\racermate\Release\common.lib
       FinalizeBuildStatus:
         Deleting file "Release\common.tlog\unsuccessfulbuild".
         Touching "Release\common.tlog\common.lastbuildstate".
     5>Done Building Project "Z:\rm6dll\common\common.vcxproj" (Rebuild target(s)).
     2>Project "Z:\rm6dll\racermate\dll.vcxproj.metaproj" (2) is building "Z:\rm6dll\racermate\dll.vcxproj" (6) on node 2 (Rebuild target(s)).
     6>_PrepareForClean:
         Deleting file "Release\racermate.tlog\racermate.lastbuildstate".
     6>Project "Z:\rm6dll\racermate\dll.vcxproj" (6) is building "Z:\rm6dll\tinythread++\tinythread++.vcxproj" (7) on node 2 (Clean target(s)).
     7>_PrepareForClean:
         Deleting file "Release\tinythread++.tlog\tinythread++.lastbuildstate".
     1>Project "Z:\rm6dll\racermate\dll_2015.sln" (1) is building "Z:\rm6dll\racermate\..\tlib\trainer_lib.vcxproj.metaproj" (4) on node 1 (Rebuild target(s)).
     4>Project "Z:\rm6dll\racermate\..\tlib\trainer_lib.vcxproj.metaproj" (4) is building "Z:\rm6dll\tlib\trainer_lib.vcxproj" (8) on node 3 (Rebuild target(s)).
     8>_PrepareForClean:
         Deleting file "Release\tlib.tlog\tlib.lastbuildstate".
     1>Project "Z:\rm6dll\racermate\dll_2015.sln" (1) is building "Z:\rm6dll\racermate\..\glib\gen_lib.vcxproj.metaproj" (3) on node 1 (Rebuild target(s)).
     3>Project "Z:\rm6dll\racermate\..\glib\gen_lib.vcxproj.metaproj" (3) is building "Z:\rm6dll\glib\gen_lib.vcxproj" (9) on node 1 (Rebuild target(s)).
     9>_PrepareForClean:
         Deleting file "Release\glib.tlog\glib.lastbuildstate".
     7>Done Building Project "Z:\rm6dll\tinythread++\tinythread++.vcxproj" (Clean target(s)).
     9>InitializeBuildStatus:
         Creating "Release\glib.tlog\unsuccessfulbuild" because "AlwaysCreate" was specified.
       ClCompile:
         C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\CL.exe /c /I../freetype/include /I../freetype/include/freetype2 /I../pcre/include /I..\ant\ANT_Windows_Library_Package_v3.0\ANT_LIB\inc /I..\ant\ANT_Windows_Library_Package_v3.0\ANT_LIB\software\serial /I..\ant\ANT_Windows_Library_Package_v3.0\ANT_LIB\software\system /I..\ant\ANT_Windows_Library_Package_v3.0\ANT_LIB\software\USB\devices /I..\ant\ANT_Windows_Library_Package_v3.0\ANT_LIB\software\USB\device_handles /I..\ant\ANT_Windows_Library_Package_v3.0\ANT_LIB\software\USB /I../common /I. /Z7 /nologo /W3 /WX- /O2 /Oi /Oy- /GL /D WIN32 /D NDEBUG /D _LIB /D _MBCS /Gm- /EHsc /MD /GS /Gy /fp:precise /Zc:wchar_t /Zc:forScope /Zc:inline /Fo"Release\\" /Fd"Release\glib.pdb" /Gd /TP /analyze- /errorReport:queue ..\common\comglobs.cpp ..\common\smav.cpp ..\common\twopolefilter.cpp amplifier.cpp averagetimer.cpp axis.cpp bmp.cpp button.cpp clientsocket.cpp crc.cpp cregexp.cpp csv.cpp dir.cpp earth.cpp editbox.cpp err.cpp fasttimer.cpp fatalerror.cpp fileselector.cpp file_rotator.cpp float2d.cpp globals.cpp ini.cpp inputdlg.cpp intel.cpp listbox.cpp logger.cpp lpfilter.cpp md5.cpp modaldialog.cpp modallistbox.cpp mru.cpp png.cpp scaler.cpp oldsdirs.cpp serial.cpp signalstrengthmeter.cpp splash.cpp statictext.cpp statusdlg.cpp tga.cpp timeout.cpp tinystr.cpp tinyxml.cpp tinyxmlerror.cpp tinyxmlparser.cpp utils.cpp utilsclass.cpp wcon.cpp win_utils.cpp windower.cpp xml.cpp
         comglobs.cpp
         smav.cpp
         twopolefilter.cpp
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
         C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\Lib.exe /OUT:"Z:\rm6dll\racermate\Release\glib.lib" /NOLOGO /MACHINE:X86 /LTCG Release\comglobs.obj
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
         gen_lib.vcxproj -> Z:\rm6dll\racermate\Release\glib.lib
       FinalizeBuildStatus:
         Deleting file "Release\glib.tlog\unsuccessfulbuild".
         Touching "Release\glib.tlog\glib.lastbuildstate".
     9>Done Building Project "Z:\rm6dll\glib\gen_lib.vcxproj" (Rebuild target(s)).
     3>Done Building Project "Z:\rm6dll\racermate\..\glib\gen_lib.vcxproj.metaproj" (Rebuild target(s)).
     8>Project "Z:\rm6dll\tlib\trainer_lib.vcxproj" (8) is building "Z:\rm6dll\tinythread++\tinythread++.vcxproj" (7:3) on node 2 (default targets).
     7>InitializeBuildStatus:
         Creating "Release\tinythread++.tlog\unsuccessfulbuild" because "AlwaysCreate" was specified.
       ClCompile:
         C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\CL.exe /c /I../../../tinythread++ /Zi /nologo /W3 /WX- /O2 /Oi /Oy- /GL /D WIN32 /D NDEBUG /D _LIB /D _MBCS /Gm- /EHsc /MD /GS /Gy /fp:precise /Zc:wchar_t /Zc:forScope /Zc:inline /Fo"Release\\" /Fd"Release\tinythread++.pdb" /Gd /TP /analyze- /errorReport:queue tinythread.cpp
         tinythread.cpp
       Lib:
         C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\Lib.exe /OUT:"Z:\rm6dll\racermate\Release\tinythread++.lib" /NOLOGO /LTCG Release\tinythread.obj
         tinythread++.vcxproj -> Z:\rm6dll\racermate\Release\tinythread++.lib
       FinalizeBuildStatus:
         Deleting file "Release\tinythread++.tlog\unsuccessfulbuild".
         Touching "Release\tinythread++.tlog\tinythread++.lastbuildstate".
     7>Done Building Project "Z:\rm6dll\tinythread++\tinythread++.vcxproj" (default targets).
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
         C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\Lib.exe /OUT:"Z:\rm6dll\racermate\Release\tlib.lib" /LIBPATH:../ant_2010/ANT_Windows_Library_Package_v3.0/ANT_LIB/Debug /NOLOGO /MACHINE:X86 /LTCG Release\bike.obj
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
         trainer_lib.vcxproj -> Z:\rm6dll\racermate\Release\tlib.lib
       FinalizeBuildStatus:
         Deleting file "Release\tlib.tlog\unsuccessfulbuild".
         Touching "Release\tlib.tlog\tlib.lastbuildstate".
     8>Done Building Project "Z:\rm6dll\tlib\trainer_lib.vcxproj" (Rebuild target(s)).
     4>Done Building Project "Z:\rm6dll\racermate\..\tlib\trainer_lib.vcxproj.metaproj" (Rebuild target(s)).
     6>InitializeBuildStatus:
         Touching "Release\racermate.tlog\unsuccessfulbuild".
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
         dll_globals.cpp
         errors.cpp
         ev.cpp
         internal.cpp
         Generating Code...
       Link:
         C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\link.exe /ERRORREPORT:QUEUE /OUT:".\Release\racermate.dll" /INCREMENTAL:NO /NOLOGO /LIBPATH:C:\Qt\5.6\msvc2015\lib /LIBPATH:..\libusb\Win32\Release\lib /LIBPATH:..\freetype\lib /LIBPATH:.\release odbc32.lib odbccp32.lib comctl32.lib winmm.lib shlwapi.lib opengl32.lib glu32.lib freetype.lib ws2_32.lib qt5core.lib qt5network.lib "libusb-1.0.lib" rmg.lib ctsrv.lib rmant.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /MANIFEST /MANIFESTUAC:"level='asInvoker' uiAccess='false'" /manifest:embed /PDB:".\Release/racermate.pdb" /MAP:".\Release/racermate.map" /OPT:REF /OPT:ICF /LTCG /TLBID:1 /DYNAMICBASE:NO /IMPLIB:".\Release/racermate.lib" /MACHINE:X86 /SAFESEH /FORCE:MULTIPLE /DLL .\Release/dll.obj
         .\Release/dll_globals.obj
         .\Release/errors.obj
         .\Release/ev.obj
         .\Release/internal.obj
         Z:\rm6dll\racermate\Release\common.lib
         Z:\rm6dll\racermate\Release\glib.lib
         Z:\rm6dll\racermate\Release\tinythread++.lib
         Z:\rm6dll\racermate\Release\tlib.lib
            Creating library .\Release/racermate.lib and object .\Release/racermate.exp
     6>MSVCRT.lib(initializers.obj) : warning LNK4098: defaultlib 'libcmt.lib' conflicts with use of other libs; use /NODEFAULTLIB:library [Z:\rm6dll\racermate\dll.vcxproj]
         Generating code
         Finished generating code
         dll.vcxproj -> Z:\rm6dll\racermate\.\Release\racermate.dll
       PostBuildEvent:
         .\release\post.bat
         :VCEnd
         **********************************  START OF ../dll/release/post.bat ***************************************
         The system cannot find the path specified.
                 0 file(s) copied.
         The system cannot find the path specified.
                 0 file(s) copied.
         The system cannot find the path specified.
                 0 file(s) copied.
         The system cannot find the path specified.
                 0 file(s) copied.
                 1 file(s) copied.
         **********************************  END OF ../dll/release/post.bat ***************************************
       FinalizeBuildStatus:
         Deleting file "Release\racermate.tlog\unsuccessfulbuild".
         Touching "Release\racermate.tlog\racermate.lastbuildstate".
     6>Done Building Project "Z:\rm6dll\racermate\dll.vcxproj" (Rebuild target(s)).
     2>Done Building Project "Z:\rm6dll\racermate\dll.vcxproj.metaproj" (Rebuild target(s)).
     1>Done Building Project "Z:\rm6dll\racermate\dll_2015.sln" (rebuild target(s)).

Build succeeded.

       "Z:\rm6dll\racermate\dll_2015.sln" (rebuild target) (1) ->
       "Z:\rm6dll\racermate\dll.vcxproj.metaproj" (Rebuild target) (2) ->
       "Z:\rm6dll\racermate\dll.vcxproj" (Rebuild target) (6) ->
       (Link target) -> 
         MSVCRT.lib(initializers.obj) : warning LNK4098: defaultlib 'libcmt.lib' conflicts with use of other libs; use /NODEFAULTLIB:library [Z:\rm6dll\racermate\dll.vcxproj]

    1 Warning(s)
    0 Error(s)

Time Elapsed 00:01:36.49
