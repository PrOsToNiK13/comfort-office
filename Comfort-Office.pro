Template = ComfortOffice

QT       += core gui
QT       += sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11


SOURCES += \
    authwindow.cpp \
    database.cpp \
    dublicatesoutput.cpp \
    dynamicFields.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    authwindow.h \
    database.h \
    dublicatesoutput.h \
    dynamicFields.h \
    mainwindow.h

FORMS += \
    authwindow.ui \
    dublicatesoutput.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    recources.qrc

win32:RC_FILE = file.rc



