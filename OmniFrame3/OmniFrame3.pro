#-------------------------------------------------
#
# Project created by QtCreator 2017-06-26T10:32:33
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OmniFrame3
TEMPLATE = app
#Puts all .o files into BIN folder
OBJECTS_DIR = ./BIN/
MOC_DIR = ./BIN/
#allows use of constexpr
QMAKE_CXXFLAGS	 += -g -Wall -std=c++0x

INCLUDEPATH += ../Sandbox \

DEPENDPATH += ../Sandbox \


SOURCES += main.cpp\
        mainwindow.cpp \
    MultipleOmnimagnetSetup.cpp \
    AMC_30A8.cpp \
    Advantech_PCI_1724U_ADIO.cpp \
    Advantech_PCI_1724U.cpp \
    Omnimagnet.cpp \
    Configuration6DOF_Producer.cpp \
    PositionProducer.cpp \
    OrientationProducer.cpp \
    ../OmniFrame2/arduino.cpp \
    Setup.cpp



HEADERS  += mainwindow.h \
    MultipleOmnimagnetSetup.h \
    AMC_30A8.h \
    Advantech_PCI_1724U_ADIO.h \
    Advantech_PCI_1724U.h \
    Omnimagnet.h \
    Configuration6DOF_Producer.h \
    PositionProducer.h \
    OrientationProducer.h \
    ../OmniFrame2/arduino.h \
    Setup.h






win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../OmnimagnetLibs/Math_Lib/release/ -lMath
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../OmnimagnetLibs/Math_Lib/debug/ -lMath
else:unix: LIBS += -L$$PWD/../OmnimagnetLibs/Math_Lib/ -lMath

INCLUDEPATH += $$PWD/../OmnimagnetLibs/Math_Lib
DEPENDPATH += $$PWD/../OmnimagnetLibs/Math_Lib

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../OmnimagnetLibs/AD_IO_Lib/release/ -lAD_IO
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../OmnimagnetLibs/AD_IO_Lib/debug/ -lAD_IO
else:unix: LIBS += -L$$PWD/../OmnimagnetLibs/AD_IO_Lib/ -lAD_IO

INCLUDEPATH += $$PWD/../OmnimagnetLibs/AD_IO_Lib
DEPENDPATH += $$PWD/../OmnimagnetLibs/AD_IO_Lib

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../OmnimagnetLibs/Producer_Lib/release/ -lProducer
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../OmnimagnetLibs/Producer_Lib/debug/ -lProducer
else:unix: LIBS += -L$$PWD/../OmnimagnetLibs/Producer_Lib/ -lProducer

INCLUDEPATH += $$PWD/../OmnimagnetLibs/Producer_Lib
DEPENDPATH += $$PWD/../OmnimagnetLibs/Producer_Lib

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../usr/lib/release/ -lcomedi
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../usr/lib/debug/ -lcomedi
else:unix: LIBS += -L$$PWD/../../../usr/lib/ -lcomedi

INCLUDEPATH += $$PWD/../../../usr/include
DEPENDPATH += $$PWD/../../../usr/include
