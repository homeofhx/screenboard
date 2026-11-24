TEMPLATE = app
TARGET = ScreenBoard
include(qmake/ScreenBoard.pri)

SOURCES += src/main.cpp src/app/Application.cpp
HEADERS += src/app/Application.h

macx: QMAKE_INFO_PLIST = $$PWD/platform/macos/Info.plist

win32: RC_ICONS = $$PWD/resources/icons/AppIcon.ico

macx {
    app_icon.files = $$PWD/resources/icons/AppIcon.icns
    app_icon.path = Contents/Resources
    QMAKE_BUNDLE_DATA += app_icon
}

unix:!android {
    target.path = /opt/ScreenBoard/bin
    INSTALLS += target
}