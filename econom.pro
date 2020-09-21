#-------------------------------------------------
#
# Project created by QtCreator 2020-09-07T08:32:13
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = econom
TEMPLATE = app

RC_FILE = ico.rc

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    import1c.cpp \
    formsrcreport.cpp \
    db/dbdelegate.cpp \
    db/dblogin.cpp \
    db/dbmapper.cpp \
    db/dbtablemodel.cpp \
    db/dbviewer.cpp \
    dialognewunload.cpp \
    dialogopen.cpp \
    treemodel.cpp \
    formnorm.cpp \
    modelro.cpp

HEADERS += \
        mainwindow.h \
    import1c.h \
    formsrcreport.h \
    db/dbdelegate.h \
    db/dblogin.h \
    db/dbmapper.h \
    db/dbtablemodel.h \
    db/dbviewer.h \
    dialognewunload.h \
    dialogopen.h \
    treemodel.h \
    formnorm.h \
    modelro.h

FORMS += \
        mainwindow.ui \
    formsrcreport.ui \
    db/dblogin.ui \
    dialognewunload.ui \
    dialogopen.ui \
    formnorm.ui

RESOURCES += \
    res.qrc
