#-------------------------------------------------
#
# Project created by QtCreator 2015-12-07T06:33:26
#
#-------------------------------------------------

QT       -= gui

TARGET = rmg
TEMPLATE = lib
CONFIG += staticlib
#CONFIG += shared

INCLUDEPATH -= .
INCLUDEPATH += ../../common \
					../../rm_common \
					../common

#		../../common/tmr.cpp \

SOURCES += \
		../../common/comutils.cpp \
		../../common/smav.cpp \
		../../common/twopolefilter.cpp \
		../common/tmr.cpp \
		../common/qtformula.cpp \
		../../common/comglobs.cpp \
		../../common/person.cpp \
		../../common/crf.cpp \
\
		qt_data.cpp \
		qt_ss.cpp

#	testdata.cpp \

#	 ../../common/tmr.h \

HEADERS += \
	../../common/comdefs.h \
	../../common/comglobs.h \
	../../common/comutils.h \
	../../common/person.h \
	 ../../common/smav.h \
	 ../../common/twopolefilter.h \
	 ../common/tmr.h \
	 ../common/qtformula.h \
\
	 ../../rm_common/rmdefs.h \
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
\
\
	 qt_data.h \
	 qt_ss.h \
	 ../../common/comdefs.h \
	 ../../common/crf.h


#	 testdata.h \

message("")
message("*********************** RMG ***************************")


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
			equals(QT_VERSION, "5.7.0")  {
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
message("CONFIG = $$CONFIG")
message("QT_VERSION = $$QT_VERSION")
