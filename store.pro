#-------------------------------------------------
#
# Project created by QtCreator 2015-07-06T14:48:41
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = store
TEMPLATE = app


SOURCES += main.cpp \
    application.cpp \
    categories.cpp \
    dialog.cpp \
    mainwindow.cpp

HEADERS  += \
    application.h \
    categories.h \
    dialog.h \
    mainwindow.h

FORMS += \
    buttons.ui \
    category.ui

RESOURCES += \
    store.qrc
