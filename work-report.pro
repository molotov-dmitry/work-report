#-------------------------------------------------
#
# Project created by QtCreator 2019-01-16T20:31:39
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = work-report
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

OBJECTS_DIR = build/obj
MOC_DIR     = build/moc
UI_DIR      = build/ui
RCC_DIR     = build/rc

RC_ICONS = res/icons/clock.ico

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    dialogtaskedit.cpp \
    plannedtasks.cpp \
    settings.cpp \
    dialogsettingsedit.cpp \
    projecttemplates.cpp \
    settingsdir.cpp \
    dialogprojecttemplatesedit.cpp \
    values.cpp \
    common/reportentry.cpp \
    common/reportexportcsv.cpp \
    common/reportimport.cpp \
    dialogprojectplan.cpp \
    monthdaysselectwidget.cpp

HEADERS += \
    mainwindow.h \
    dialogtaskedit.h \
    plannedtasks.h \
    settings.h \
    dialogsettingsedit.h \
    projecttemplates.h \
    settingsdir.h \
    dialogprojecttemplatesedit.h \
    values.h \
    common/reportentry.h \
    common/reportexportcsv.h \
    common/reportimport.h \
    dialogprojectplan.h \
    monthdaysselectwidget.h

FORMS += \
    mainwindow.ui \
    dialogtaskedit.ui \
    dialogsettingsedit.ui \
    dialogprojecttemplatesedit.ui \
    dialogprojectplan.ui \
    monthdaysselectwidget.ui

RESOURCES += \
    res/resources.qrc

unix: target.path = /usr/local/bin
!isEmpty(target.path): INSTALLS += target

unix: desktop.path = /usr/share/applications
unix: desktop.files = work-report.desktop

!isEmpty(desktop.path): INSTALLS += desktop
