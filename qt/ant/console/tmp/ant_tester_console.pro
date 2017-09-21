
TEMPLATE = app
QT       += core network
QT       -= gui
#CONFIG += c++11

#SOURCES +=	main.cpp \
#				SleepDetector.cpp

INCLUDEPATH += \
				../../../libusb-1.0.20/libusb \
				../

#HEADERS +=	\
#				ant.h \
#				SleepDetector.h \
#				../tmr.h \
#				../comdefs.h \
#				../antchan.h \
#				../antdev.h \
#				../antmsg.h \
#				../antmessages.h

INCLUDEPATH += .

HEADERS +=	\
				consolereader.h \
				tmr.h \
				comdefs.h \
				ant.h

SOURCES +=	main.cpp \
				consolereader.cpp \
				tmr.cpp

message("")
message("*********************** console ***************************")


# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =


message("")
message("*********************** ant_tester ***************************")


CONFIG(debug, debug|release) {
	message( debug )
	DEFINES += _DEBUG

	unix  {
		!mac:!android  {
			message("pure unix")
			#QMAKE_CXXFLAGS += -std=c++11
			LIBS += -L$$PWD/../../../libusb-1.0.20/libusb/.libs -lusb-1.0
			LIBS += -L$$PWD/../Desktop_Qt_5_6_1_GCC_64bit-Debug -lrmant

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
		LIBS += -L$$PWD/../../../libusb-win32-bin-1.0.20/MS32/static -lusb-1.0
		#LIBS += -L$$PWD/../../libusb-win32-bin-1.0.20/MS32/static
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

# Default rules for deployment.
include(deployment.pri)
