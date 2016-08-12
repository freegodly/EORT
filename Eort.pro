#-------------------------------------------------
#
# Project created by QtCreator 2014-07-08T10:17:55
#
#-------------------------------------------------

QT       += core gui xml

#QMAKE_CXXFLAGS += -std=c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Eort
TEMPLATE = app



LIBS += ../Eort/3rd/libxl/lib/libxl.lib

#LIBS += ../Eort/3rd/libxl/lib/liblibxl.a

INCLUDEPATH += 3rd  \
     3rd/libxl/include_cpp

SOURCES += main.cpp\
        mainwindow.cpp \
    modules/i_modules.cpp \
    form_clock.cpp \
    form_humidity.cpp \
    etest.cpp \
    flow_result.cpp \
    global.cpp \
    3rd/qcustomplot.cpp \
    dialog_setting.cpp

HEADERS  += mainwindow.h \
    modules/i_modules.h \
    form_clock.h \
    form_humidity.h \
    qcustomplot.h \
    etest.h \
    flow_result.h \
    global.h \
    3rd/qcustomplot.h \
    dialog_setting.h \
    3rd/libxl/include_cpp/enum.h \
    3rd/libxl/include_cpp/IBookT.h \
    3rd/libxl/include_cpp/IFontT.h \
    3rd/libxl/include_cpp/IFormatT.h \
    3rd/libxl/include_cpp/ISheetT.h \
    3rd/libxl/include_cpp/libxl.h \
    3rd/libxl/include_cpp/setup.h

FORMS    += mainwindow.ui \
    form_clock.ui \
    form_humidity.ui \
    form_chart.ui \
    dialog_setting.ui

RESOURCES += \
    res/res.qrc
RC_FILE += \
    icon.rc

