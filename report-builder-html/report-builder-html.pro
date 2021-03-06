QT       += core xml

TARGET   = report-builder-html
TEMPLATE = app

DEFINES  += QT_DEPRECATED_WARNINGS

INCLUDEPATH += .. ../common

OBJECTS_DIR = build/obj
MOC_DIR     = build/moc
UI_DIR      = build/ui
RCC_DIR     = build/rc

SOURCES += ../values.cpp \
    ../settings.cpp \
    ../settingsdir.cpp \
    report-builder-html.cpp \
    ../common/reportentry.cpp \
    ../common/reportexportcsv.cpp \
    ../common/reportimport.cpp

HEADERS += ../values.h \
    ../common/reportentry.h \
    ../common/reportexportcsv.h \
    ../common/reportimport.h \
    ../settings.h \
    ../settingsdir.h

unix: target.path = /usr/local/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res/resources.qrc
