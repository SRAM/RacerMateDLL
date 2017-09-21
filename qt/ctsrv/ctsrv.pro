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

INCLUDEPATH +=		../rmg \
						../common \
						../../common \
						../../rm_common \
						../computrainer

SOURCES +=	ctsrv.cpp \
				udp_client.cpp \
				ctsrvworker.cpp \
				../common/sdirs.cpp \
				../common/tmr.cpp


HEADERS +=	ctsrv.h \
				udp_client.h \
				ctsrvworker.h \
				../common/sdirs.h \
				../common/tmr.h \
				../../rm_common/rmconst.h \
				../../rm_common/coursefile.h \
				../../rm_common/enums.h \
				../../rm_common/metadata.h \
				../../rm_common/nv.h \
				../../rm_common/public.h \
				../../rm_common/rider_data.h \
				../../rm_common/rider2.h \
				../../rm_common/ssdata.h \
				../../rm_common/trainerdata.h \
				../../rm_common/rmdefs.h


message("")
message("*********************** ctsrv ***************************")


CONFIG(debug, debug|release) {
	message( debug )
	DEFINES += _DEBUG

	unix  {

		!mac:!android  {
			message("pure unix")
			LIBS += -L$$PWD/../rmg/Desktop_Qt_5_6_1_GCC_64bit-Debug -lrmg
			target.path = /usr/lib
			INSTALLS += target
		}
		mac  {
			macx  {
				message("mac")
				equals(QT_VERSION, "5.5.1")  {
				}
				equals(QT_VERSION, "5.6.0")  {
				}
				equals(QT_VERSION, "5.7.0")  {
					LIBS += -L$$PWD/../rmg/build-rmg-Desktop_Qt_5_7_0_clang_64bit-Debug -lrmg
				}
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
		LIBS =
		!contains(QMAKE_TARGET.arch, x86_64) {
			message("x86 (32 BIT) build")
			equals(QT_VERSION, "5.5.1")  {
				LIBS += -L$$PWD/../rmg/551-2012-32/debug -lrmg
			}
			equals(QT_VERSION, "5.6.0")  {
				#LIBS += -L$$PWD/../rmg/560_2015-32/debug -lrmg
				LIBS += -L$$PWD/../rmg/Desktop_Qt_5_6_0_MSVC2015_32bit/debug -lrmg
			}
		}
		else {
			message("x86_64 (64 BIT) build")

			equals(QT_VERSION, "5.5.1")  {
				LIBS += -L$$PWD/../rmg/551-2012-64/debug -lrmg
			}
			equals(QT_VERSION, "5.6.0")  {
				LIBS += -L$$PWD/../rmg/560-2015-64/debug -lrmg
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
			LIBS += -L$$PWD/../rmg/Desktop_Qt_5_6_1_GCC_64bit-Release -lrmg
		}
		mac  {
			macx  {
				message("mac")
				equals(QT_VERSION, "5.5.1")  {
				}
				equals(QT_VERSION, "5.6.0")  {
				}
				equals(QT_VERSION, "5.7.0")  {
					LIBS += -L$$PWD/../rmg/build-rmg-Desktop_Qt_5_7_0_clang_64bit-Release -lrmg
				}
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

		!contains(QMAKE_TARGET.arch, x86_64) {
			message("x86 (32 BIT) build")

			equals(QT_VERSION, "5.5.1")  {
				LIBS += -L$$PWD/../../rmg/xxx/release -lrmg
			}
			equals(QT_VERSION, "5.6.0")  {
				LIBS += -L$$PWD/../rmg/Desktop_Qt_5_6_0_MSVC2015_32bit/release -lrmg
			}
		}
		else {
			message("x86_64 (64 BIT) build")
			equals(QT_VERSION, "5.5.1")  {
				LIBS += -L$$PWD/../../rmg/xxx/debug -lrmg
			}
			equals(QT_VERSION, "5.6.0")  {
				LIBS += -L$$PWD/../../rmg/xxx/debug -lrmg
			}
		}

	}

}


message("DEFINES = $$DEFINES")
message("PWD = $$PWD")
message("INCLUDEPATH = $$INCLUDEPATH")
message("LIBS = $$LIBS")
message("CONFIG = $$CONFIG")
message("QT_VERSION = $$QT_VERSION")
