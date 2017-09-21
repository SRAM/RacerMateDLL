#-------------------------------------------------
#
# Project created by QtCreator 2015-08-21T03:01:32
#
#-------------------------------------------------

QT       += network
QT       -= gui


TARGET = computrainer
TEMPLATE = lib
#CONFIG += shared
CONFIG += staticlib

INCLUDEPATH += \
					../../common \
					../../rm_common \
					../rmg \
					../ctsrv

SOURCES +=	computrainer.cpp

HEADERS +=	computrainer.h


message("")
message("*********************** Computrainer ***************************")


CONFIG(debug, debug|release) {
	message( debug )
		DEFINES += _DEBUG

	unix  {
		LIBS += -L$$PWD/../rmg/build-rmg-Desktop_Qt_5_5_0_GCC_64bit-Debug -lrmg
		LIBS += -L$$PWD/../ctsrv/build-ctsrv-Desktop_Qt_5_5_0_GCC_64bit-Debug -lctsrv

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
		LIBS += -L$$PWD/../ctsrv/build-ctsrv-Desktop_Qt_5_5_0_MSVC2012_32bit/debug -lctsrv
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
		LIBS += -L$$PWD/../ctsrv/build-ctsrv-Desktop_Qt_5_5_0_MSVC2012_32bit/release -lctsrv
	}
}@


message("DEFINES = $$DEFINES")
message("PWD = $$PWD")
message("INCLUDEPATH = $$INCLUDEPATH")
message("LIBS = $$LIBS")
