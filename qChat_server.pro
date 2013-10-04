
#-------------------------------------------------
#
# Project created by QtCreator 2013-08-23T01:26:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network

TARGET = qChat_server
TEMPLATE = app
target.path = /home/makak/qchat
INSTALLS += target


SOURCES += main.cpp\
        dialog.cpp \
    server.cpp \
    client.cpp \
    blowfish.cpp

HEADERS  += dialog.h \
    server.h \
    client.h \
    blowfish.h

FORMS    += dialog.ui
