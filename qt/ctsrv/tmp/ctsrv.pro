#-------------------------------------------------
#
# Project created by QtCreator 2015-12-07T06:13:03
#
#-------------------------------------------------

QT       += network

QT       -= gui

TARGET = ctsrv
TEMPLATE = lib
CONFIG += staticlib
#CONFIG += shared

INCLUDEPATH += ../rmg \
					../../../common \
					../../rm_common \
					../computrainer

SOURCES += ctsrv.cpp \
	 udp_client.cpp \
    worker.cpp

HEADERS += ctsrv.h \
	 udp_client.h \
    ../../rm_common/bike.h \
	 ../../rm_common/rmconst.h \
    ../../rm_common/course.h \
    ../../rm_common/coursefile.h \
    ../../rm_common/datasource.h \
    ../../rm_common/device.h \
    ../../rm_common/enums.h \
    ../../rm_common/metadata.h \
    ../../rm_common/nv.h \
    ../../rm_common/public.h \
    ../../rm_common/rider_data.h \
    ../../rm_common/rider2.h \
    ../../rm_common/ssdata.h \
    ../../rm_common/trainerdata.h \
    ../../rm_common/rmdefs.h \
    worker.h


message("")
message("*********************** ctsrv ***************************")


CONFIG(debug, debug|release) {
	message( debug )
	DEFINES += _DEBUG

	unix  {
		LIBS += -L$$PWD/../rmg/build-rmg-Desktop_Qt_5_5_0_GCC_64bit-Debug -lrmg

		!mac:!android  {
			message("pure unix")
			target.path = /usr/lib
			INSTALLS += target
		}
		mac  {
			macx  {
				message("mac")
			}
			!macx  {
				message("ios")
			}
		}
		android  {
			message("android")
		}
	}
	win32  {
		message("win32")
		LIBS += -L$$PWD/../rmg/build-rmg-Desktop_Qt_5_5_0_MSVC2012_32bit/debug -lrmg
	}
}
else  {
	message( release)
	DEFINES -= _DEBUG
	unix  {
		!mac:!android  {
			message("pure unix")
		}
		mac  {
			macx  {
				message("mac")
			}
			!macx  {
				message("ios")
			}
		}
		android  {
			message("android")
		}
	}
	win32  {
		message("win32")
		LIBS += -L$$PWD/../rmg/build-rmg-Desktop_Qt_5_5_0_MSVC2012_32bit/release -lrmg
	}
}


message("DEFINES = $$DEFINES")
message("PWD = $$PWD")
message("INCLUDEPATH = $$INCLUDEPATH")
message("LIBS = $$LIBS")
