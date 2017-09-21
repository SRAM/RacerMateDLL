
TEMPLATE = app
QT       += core
QT       -= gui

CONFIG += c++11
CONFIG += console
CONFIG -= app_bundle



INCLUDEPATH += \
				../../../libusb/libusb \
				../../../common \
				../../common \
				../ \
				.

HEADERS +=	\
				../../../common/comdefs.h \
				../../common/consolereader.h \
				../../common/tmr.h \
				../../common/sdirs.h \
				mainclass.h

SOURCES +=	\
				../../common/tmr.cpp \
				../../common/sdirs.cpp \
				../../common/consolereader.cpp \
				mainclass.cpp \
				main.cpp



message("")
message("*********************** antcon ***************************")


CONFIG(debug, debug|release) {
	message( debug )
	DEFINES += _DEBUG

	unix  {
		!mac:!android  {
			message("pure unix")
			#QMAKE_CXXFLAGS_DEBUG += -Og
			QMAKE_CXXFLAGS_DEBUG += -O0
			LIBS += -L$$PWD/../Desktop_Qt_5_6_1_GCC_64bit-Debug -lrmant
			LIBS += -L$$PWD/../../../libusb/libusb/.libs -lusb-1.0
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
		LIBS =
		!contains(QMAKE_TARGET.arch, x86_64) {
			#QMAKE_CXXFLAGS_DEBUG += /MTd
			LIBS += -L$$PWD/../../../libusb/Win32/Debug/lib -llibusb-1.0
			LIBS += -L$$PWD/../Desktop_Qt_5_6_0_MSVC2015_32bit/debug -lrmant
			LIBS += -L$$PWD/../../mslibs/32bit -lshell32
		}
		else {
			message("x86_64 (64 BIT) build")
		}
	}
}
else  {
	message( release)
	DEFINES -= _DEBUG
	unix  {
		!mac:!android  {
			message("pure unix")
			LIBS += -L$$PWD/../Desktop_Qt_5_6_1_GCC_64bit-Release -lrmant
			LIBS += -L$$PWD/../../../libusb/libusb/.libs -lusb-1.0
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
		LIBS += -L$$PWD/../../../libusb/Win32/Release/lib/ -llibusb-1.0
		LIBS += -L$$PWD/../Desktop_Qt_5_6_0_MSVC2015_32bit/release -lrmant
		LIBS += -L$$PWD/../../mslibs/32bit -lshell32
	}
}


message("DEFINES = $$DEFINES")
message("PWD = $$PWD")
message("INCLUDEPATH = $$INCLUDEPATH")
message("LIBS = $$LIBS")

# Default rules for deployment.
#include(deployment.pri)

