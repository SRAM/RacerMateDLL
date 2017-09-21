::@copy ..\..\qt\ctsrv\build-ctsrv-Desktop_Qt_5_5_0_MSVC2012_32bit\debug\ctsrv.lib
::@copy ..\..\qt\rmg\build-rmg-Desktop_Qt_5_5_0_MSVC2012_32bit\debug\rmg.lib

dumpbin /exports rmg.lib >rmg.x
dumpbin /exports ctsrv.lib >ctsrv.x

