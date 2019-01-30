QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = work-report
TEMPLATE = app


DEFINES += QT_DEPRECATED_WARNINGS


OBJECTS_DIR = build/obj
MOC_DIR     = build/moc
UI_DIR      = build/gen_ui

RC_ICONS = res/icons/clock.ico

SOURCES += values.cpp \
    aggregator.cpp

HEADERS += values.h
