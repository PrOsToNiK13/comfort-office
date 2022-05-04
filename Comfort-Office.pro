Template = ComfortOffice

QT       += core gui
QT       += sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11


SOURCES += \
    auth.cpp \
    database.cpp \
    dynamicFields.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    auth.h \
    database.h \
    dynamicFields.h \
    mainwindow.h

FORMS += \
    auth.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
