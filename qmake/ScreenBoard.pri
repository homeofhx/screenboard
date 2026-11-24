QT += core gui widgets svg
CONFIG += c++17

SCREENBOARD_ROOT = $$clean_path($$PWD/..)
INCLUDEPATH += $$SCREENBOARD_ROOT/src

include(core.pri)
include(services.pri)
include(ui.pri)

RESOURCES += $$SCREENBOARD_ROOT/resources/resources.qrc