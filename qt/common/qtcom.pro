#-------------------------------------------------
#
# Project created by QtCreator 2015-12-07T06:33:26
#
#-------------------------------------------------

QT       -= gui

TARGET = qtcom
TEMPLATE = lib
CONFIG += staticlib
CONFIG += c++11

INCLUDEPATH += .
INCLUDEPATH += ../../common

SOURCES += \
	tmr.cpp \
	consolereader.cpp


HEADERS += \
	../../common/comdefs.h \
	 tmr.h \
	 consolereader.h

message("")
message("*********************** common ***************************")


CONFIG(debug, debug|release) {
	message( debug )
	DEFINES += _DEBUG

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
		!contains(QMAKE_TARGET.arch, x86_64) {
			message("x86 (32 BIT) build")
		}
		else {
			message("x86_64 (64 BIT) build")
			#DEFINES -= WIN32
			equals(QT_VERSION, "5.5.1")  {
			}
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
	}
}

message("DEFINES = $$DEFINES")
message("PWD = $$PWD")
message("INCLUDEPATH = $$INCLUDEPATH")
message("LIBS = $$LIBS")

