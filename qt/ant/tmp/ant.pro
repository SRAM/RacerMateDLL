#-------------------------------------------------
#
# Project created by QtCreator 2016-07-13T09:59:37
#
#-------------------------------------------------

QT       -= gui

TARGET = rmant
TEMPLATE = lib

#DEFINES += SHAREDLIB_LIBRARY
#DEFINES += STATIC_LIBRARY
#CONFIG += shared

CONFIG += staticlib

CONFIG += c++11

INCLUDEPATH += \
				../../libusb/libusb \
				../../common \
				../common \
				.

SOURCES +=	\
				antdev.cpp \
				../../common/smav.cpp \
				../../common/median.cpp \
				../common/tmr.cpp \
				../common/utils.cpp \
				../common/sdirs.cpp \
				antworker.cpp \
				ant.cpp \
				antchan.cpp


HEADERS +=	\
				../../common/comdefs.h \
				../../common/smav.h \
				../../common/median.h \
				../common/tmr.h \
				../common/sdirs.h \
				../common/utils.h \
				sl_global.h \
				antdefs.h \
				antchan.h \
				antdev.h \
				anterrs.h \
				antworker.h \
				ant.h


message("")
message("*********************** ANT ***************************")


CONFIG(debug, debug|release) {
	message( debug )
	DEFINES += _DEBUG

	unix  {


	 !mac:!android  {
			message("pure unix")
			QMAKE_CXXFLAGS_DEBUG += -O0
			target.path = /usr/lib
			INSTALLS += target
			LIBS += -L$$PWD/../../libusb/libusb/.libs -lusb-1.0
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
		message("windows")
		#DEFINES -= MDd
		#QMAKE_CXXFLAGS_DEBUG += /MTd
		#QMAKE_LFLAGS_DEBUG += /NODEFAULTLIB:LIBCMTD
		LIBS =
		!contains(QMAKE_TARGET.arch, x86_64) {
			message("x86 (32 BIT) build")
			equals(QT_VERSION, "5.6.0")  {
				LIBS += -L$$PWD/../../libusb/Win32/Debug/lib -llibusb-1.0
				#LIBS += -L$$PWD/../../libusb/Win32/Debug/dll -llibusb-1.0
				LIBS += -L$$PWD/../mslibs/32bit -lshell32
			}
		}
		else {
			message("x86_64 (64 BIT) build")
			#DEFINES -= WIN32
			equals(QT_VERSION, "5.6.0")  {
			}
		}
	}
}
else  {

	message( release)
	DEFINES -= _DEBUG
	unix  {
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
		message("windows")
		QMAKE_LFLAGS_RELEASE += /NODEFAULTLIB:LIBCMT
		#QMAKE_CXXFLAGS_RELEASE += /MT
		!contains(QMAKE_TARGET.arch, x86_64) {
			message("x86 (32 BIT) build")
			equals(QT_VERSION, "5.6.0")  {
				LIBS += -L$$PWD/../../libusb/Win32/Release/lib -llibusb-1.0
				LIBS += -L$$PWD/../../mslibs/32bit -lshell32
			}
		}
		else {
			message("x86_64 (64 BIT) build")
			#DEFINES -= WIN32
			equals(QT_VERSION, "5.6.0")  {
			}
		}
	}
}

message("DEFINES = $$DEFINES")
message("PWD = $$PWD")
message("INCLUDEPATH = $$INCLUDEPATH")
message("LIBS = $$LIBS")
message("QT_VERSION = $$QT_VERSION")


