QT       += core

TARGET   = report-builder
TEMPLATE = app

DEFINES  += QT_DEPRECATED_WARNINGS

INCLUDEPATH += ..

OBJECTS_DIR = build/report-builder/obj
MOC_DIR     = build/report-builder/moc
UI_DIR      = build/report-builder/gen_ui

SOURCES += ../values.cpp \
    report-builder.cpp \
    reportbuilderstatistics.cpp \
    reportbuildertotalbydate.cpp \
    reportbuildertotalreduced.cpp \
    reportentry.cpp

HEADERS += ../values.h \
    reportbuilderstatistics.h \
    reportbuildertotalbydate.h \
    reportbuildertotalreduced.h \
    reportentry.h
