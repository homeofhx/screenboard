SOURCES += \
    $$SCREENBOARD_ROOT/src/ui/ScreenUtils.cpp \
    $$SCREENBOARD_ROOT/src/ui/canvas/CanvasWidget.cpp \
    $$SCREENBOARD_ROOT/src/ui/canvas/CanvasWindow.cpp \
    $$SCREENBOARD_ROOT/src/ui/floating/FloatingButton.cpp \
    $$SCREENBOARD_ROOT/src/ui/toolbar/ColorPickerPopup.cpp \
    $$SCREENBOARD_ROOT/src/ui/toolbar/SizeSliderWidget.cpp \
    $$SCREENBOARD_ROOT/src/ui/toolbar/ToolBar.cpp \
    $$SCREENBOARD_ROOT/src/ui/tray/TrayManager.cpp

HEADERS += \
    $$SCREENBOARD_ROOT/src/ui/ScreenUtils.h \
    $$SCREENBOARD_ROOT/src/ui/canvas/CanvasWidget.h \
    $$SCREENBOARD_ROOT/src/ui/canvas/CanvasWindow.h \
    $$SCREENBOARD_ROOT/src/ui/floating/FloatingButton.h \
    $$SCREENBOARD_ROOT/src/ui/toolbar/ColorPickerPopup.h \
    $$SCREENBOARD_ROOT/src/ui/toolbar/SizeSliderWidget.h \
    $$SCREENBOARD_ROOT/src/ui/toolbar/ToolBar.h \
    $$SCREENBOARD_ROOT/src/ui/tray/TrayManager.h

macx {
    HEADERS += $$SCREENBOARD_ROOT/platform/macos/MacWindowUtils.h
    OBJECTIVE_SOURCES += $$SCREENBOARD_ROOT/platform/macos/MacWindowUtils.mm
    LIBS += -framework AppKit
}