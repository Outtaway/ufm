QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    Content.cpp \
    ContentContextMenu.cpp \
    PathChain.cpp \
    PathLine.cpp \
    QuickAccessSection.cpp \
    QuickPanel.cpp \
    RecentSection.cpp \
    SectionBase.cpp \
    Settings.cpp \
    SettingsButton.cpp \
    SettingsWindow.cpp \
    Window.cpp \
    main.cpp

HEADERS += \
    Content.h \
    ContentContextMenu.h \
    PathChain.h \
    PathLine.h \
    QuickAccessSection.h \
    QuickPanel.h \
    RecentSection.h \
    SectionBase.h \
    Settings.h \
    SettingsButton.h \
    SettingsWindow.h \
    Window.h

FORMS += \
    SettingsWindow.ui \
    Window.ui

RESOURCES = application.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
