QT       += core

TARGET   = report-builder
TEMPLATE = app

DEFINES  += QT_DEPRECATED_WARNINGS

INCLUDEPATH += .. ../common

OBJECTS_DIR = build/obj
MOC_DIR     = build/moc
UI_DIR      = build/ui
RCC_DIR     = build/rc

SOURCES += ../values.cpp \
    report-builder.cpp \
    reportbuilderstatistics.cpp \
    reportbuildertotalbydate.cpp \
    reportbuildertotalreduced.cpp \
    ../common/reportentry.cpp \
    ../common/reportimport.cpp

HEADERS += ../values.h \
    reportbuilderstatistics.h \
    reportbuildertotalbydate.h \
    reportbuildertotalreduced.h \
    ../common/reportentry.h \
    ../common/reportimport.h

unix: target.path = /usr/local/bin
!isEmpty(target.path): INSTALLS += target
