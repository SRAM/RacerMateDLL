# Microsoft Developer Studio Project File - Name="glib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=glib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "glib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "glib.mak" CFG="glib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "glib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "glib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "glib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "glib___Win32_Release"
# PROP BASE Intermediate_Dir "glib___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "glib___Win32_Release"
# PROP Intermediate_Dir "glib___Win32_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "glib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "glib___Win32_Debug"
# PROP BASE Intermediate_Dir "glib___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "glib___Win32_Debug"
# PROP Intermediate_Dir "glib___Win32_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ  /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\freetype\include" /I "..\freetype\include\freetype2" /I "." /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /GZ  /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "glib - Win32 Release"
# Name "glib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\amplifier.cpp
# End Source File
# Begin Source File

SOURCE=.\amplifier.h
# End Source File
# Begin Source File

SOURCE=.\averagetimer.cpp
# End Source File
# Begin Source File

SOURCE=.\averagetimer.h
# End Source File
# Begin Source File

SOURCE=.\axis.cpp
# End Source File
# Begin Source File

SOURCE=.\axis.h
# End Source File
# Begin Source File

SOURCE=.\beep2.h
# End Source File
# Begin Source File

SOURCE=.\beepsound.h
# End Source File
# Begin Source File

SOURCE=.\bmp.cpp
# End Source File
# Begin Source File

SOURCE=.\bmp.h
# End Source File
# Begin Source File

SOURCE=.\button.cpp
# End Source File
# Begin Source File

SOURCE=.\button.h
# End Source File
# Begin Source File

SOURCE=.\camera.h
# End Source File
# Begin Source File

SOURCE=.\chan.cpp
# End Source File
# Begin Source File

SOURCE=.\chan.h
# End Source File
# Begin Source File

SOURCE=.\clientsocket.cpp
# End Source File
# Begin Source File

SOURCE=.\clientsocket.h
# End Source File
# Begin Source File

SOURCE=.\console.cpp
# End Source File
# Begin Source File

SOURCE=.\console.h
# End Source File
# Begin Source File

SOURCE=.\crc.cpp
# End Source File
# Begin Source File

SOURCE=.\crc.h
# End Source File
# Begin Source File

SOURCE=.\cregexp.cpp
# End Source File
# Begin Source File

SOURCE=.\cregexp.h
# End Source File
# Begin Source File

SOURCE=.\crf.cpp
# End Source File
# Begin Source File

SOURCE=.\crf.h
# End Source File
# Begin Source File

SOURCE=.\csv.cpp
# End Source File
# Begin Source File

SOURCE=.\csv.h
# End Source File
# Begin Source File

SOURCE=.\defines.h
# End Source File
# Begin Source File

SOURCE=.\dir.cpp
# End Source File
# Begin Source File

SOURCE=.\dir.h
# End Source File
# Begin Source File

SOURCE=.\editbox.cpp
# End Source File
# Begin Source File

SOURCE=.\editbox.h
# End Source File
# Begin Source File

SOURCE=.\err.cpp
# End Source File
# Begin Source File

SOURCE=.\err.h
# End Source File
# Begin Source File

SOURCE=.\fasttimer.cpp
# End Source File
# Begin Source File

SOURCE=.\fasttimer.h
# End Source File
# Begin Source File

SOURCE=.\fatalerror.cpp
# End Source File
# Begin Source File

SOURCE=.\fatalerror.h
# End Source File
# Begin Source File

SOURCE=.\fileselector.cpp
# End Source File
# Begin Source File

SOURCE=.\fileselector.h
# End Source File
# Begin Source File

SOURCE=.\float2d.cpp
# End Source File
# Begin Source File

SOURCE=.\float2d.h
# End Source File
# Begin Source File

SOURCE=.\font.cpp
# End Source File
# Begin Source File

SOURCE=.\font.h
# End Source File
# Begin Source File

SOURCE=.\freetype_font.cpp
# End Source File
# Begin Source File

SOURCE=.\freetype_font.h
# End Source File
# Begin Source File

SOURCE=.\gentypes.h
# End Source File
# Begin Source File

SOURCE=.\glavi.cpp
# End Source File
# Begin Source File

SOURCE=.\glavi.h
# End Source File
# Begin Source File

SOURCE=.\globals.cpp
# End Source File
# Begin Source File

SOURCE=.\globals.h
# End Source File
# Begin Source File

SOURCE=.\ini.cpp
# End Source File
# Begin Source File

SOURCE=.\ini.h
# End Source File
# Begin Source File

SOURCE=.\listbox.cpp
# End Source File
# Begin Source File

SOURCE=.\listbox.h
# End Source File
# Begin Source File

SOURCE=.\logger.cpp
# End Source File
# Begin Source File

SOURCE=.\logger.h
# End Source File
# Begin Source File

SOURCE=.\lpfilter.cpp
# End Source File
# Begin Source File

SOURCE=.\lpfilter.h
# End Source File
# Begin Source File

SOURCE=.\md5.cpp
# End Source File
# Begin Source File

SOURCE=.\md5.h
# End Source File
# Begin Source File

SOURCE=.\minmax.h
# End Source File
# Begin Source File

SOURCE=.\modaldialog.cpp
# End Source File
# Begin Source File

SOURCE=.\modaldialog.h
# End Source File
# Begin Source File

SOURCE=.\modallistbox.cpp
# End Source File
# Begin Source File

SOURCE=.\modallistbox.h
# End Source File
# Begin Source File

SOURCE=.\model.h
# End Source File
# Begin Source File

SOURCE=.\mru.cpp
# End Source File
# Begin Source File

SOURCE=.\mru.h
# End Source File
# Begin Source File

SOURCE=.\ogl.cpp
# End Source File
# Begin Source File

SOURCE=.\ogl.h
# End Source File
# Begin Source File

SOURCE=.\person.cpp
# End Source File
# Begin Source File

SOURCE=.\person.h
# End Source File
# Begin Source File

SOURCE=.\png.cpp
# End Source File
# Begin Source File

SOURCE=.\png.h
# End Source File
# Begin Source File

SOURCE=.\ref.h
# End Source File
# Begin Source File

SOURCE=.\round.h
# End Source File
# Begin Source File

SOURCE=.\scaler.cpp
# End Source File
# Begin Source File

SOURCE=.\scaler.h
# End Source File
# Begin Source File

SOURCE=.\seh.h
# End Source File
# Begin Source File

SOURCE=.\serial.cpp
# End Source File
# Begin Source File

SOURCE=.\serial.h
# End Source File
# Begin Source File

SOURCE=.\signalstrengthmeter.cpp
# End Source File
# Begin Source File

SOURCE=.\signalstrengthmeter.h
# End Source File
# Begin Source File

SOURCE=.\smav.cpp
# End Source File
# Begin Source File

SOURCE=.\smav.h
# End Source File
# Begin Source File

SOURCE=.\statictext.cpp
# End Source File
# Begin Source File

SOURCE=.\statictext.h
# End Source File
# Begin Source File

SOURCE=.\statusdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\statusdlg.h
# End Source File
# Begin Source File

SOURCE=.\tga.cpp
# End Source File
# Begin Source File

SOURCE=.\tga.h
# End Source File
# Begin Source File

SOURCE=.\tinystr.cpp
# End Source File
# Begin Source File

SOURCE=.\tinystr.h
# End Source File
# Begin Source File

SOURCE=.\tinyxml.cpp
# End Source File
# Begin Source File

SOURCE=.\tinyxml.h
# End Source File
# Begin Source File

SOURCE=.\tinyxmlerror.cpp
# End Source File
# Begin Source File

SOURCE=.\tinyxmlparser.cpp
# End Source File
# Begin Source File

SOURCE=.\twopolefilter.cpp
# End Source File
# Begin Source File

SOURCE=.\twopolefilter.h
# End Source File
# Begin Source File

SOURCE=.\types.h
# End Source File
# Begin Source File

SOURCE=.\utils.cpp
# End Source File
# Begin Source File

SOURCE=.\utils.h
# End Source File
# Begin Source File

SOURCE=.\utilsclass.cpp
# End Source File
# Begin Source File

SOURCE=.\utilsclass.h
# End Source File
# Begin Source File

SOURCE=.\vector.h
# End Source File
# Begin Source File

SOURCE=.\win_fasttimer.h
# End Source File
# Begin Source File

SOURCE=.\win_freetype.cpp
# End Source File
# Begin Source File

SOURCE=.\win_freetype.h
# End Source File
# Begin Source File

SOURCE=.\win_ogl.cpp
# End Source File
# Begin Source File

SOURCE=.\win_ogl.h
# End Source File
# Begin Source File

SOURCE=.\win_serial.h
# End Source File
# Begin Source File

SOURCE=.\win_utils.cpp
# End Source File
# Begin Source File

SOURCE=.\win_utils.h
# End Source File
# Begin Source File

SOURCE=.\xml.cpp
# End Source File
# Begin Source File

SOURCE=.\xml.h
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# End Target
# End Project
