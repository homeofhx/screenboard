#include "MacWindowUtils.h"
#include <QWidget>
#import <AppKit/AppKit.h>

namespace {
NSWindow *nativeWindow(QWidget *widget) {
    if (!widget) return nil;

    // QWidget::winId(): NSView pointer on macOS. Calling it also ensures Qt has created the native view and its containing NSWindow
    NSView *view = reinterpret_cast<NSView *>(widget->winId());
    return view.window;
}
}

namespace MacWindowUtils {
void showFloatingWindow(QWidget *widget) {
    @autoreleasepool {
        NSWindow *window = nativeWindow(widget);
        if (!window) return;

        // Qt::Tool is backed by an NSPanel, which macOS normally removes when its application deactivates. The launcher
        // is intentionally a non-activating, persistent overlay instead
        window.hidesOnDeactivate = NO;
        window.level = NSFloatingWindowLevel;

        NSWindowCollectionBehavior behavior = window.collectionBehavior;
        behavior &= ~NSWindowCollectionBehaviorMoveToActiveSpace;
        behavior |= NSWindowCollectionBehaviorCanJoinAllSpaces | NSWindowCollectionBehaviorFullScreenAuxiliary;
        window.collectionBehavior = behavior;

        if ([window isKindOfClass:[NSPanel class]]) {
            NSPanel *panel = (NSPanel *)window;
            panel.styleMask |= NSWindowStyleMaskNonactivatingPanel;
            panel.floatingPanel = YES;
            panel.becomesKeyOnlyIfNeeded = YES;
            panel.worksWhenModal = YES;
        }

        [window orderFrontRegardless];
    }
}

qint64 frontmostExternalApplicationPid() {
    @autoreleasepool {
        NSRunningApplication *application = NSWorkspace.sharedWorkspace.frontmostApplication;
        if (!application) return 0;

        const pid_t pid = application.processIdentifier;
        if (pid == NSProcessInfo.processInfo.processIdentifier) return 0;
        return static_cast<qint64>(pid);
    }
}

void activateApplication(qint64 pid) {
    if (pid <= 0) return;

    @autoreleasepool {
        NSRunningApplication *application =
            [NSRunningApplication runningApplicationWithProcessIdentifier:static_cast<pid_t>(pid)];
        if (!application || application.terminated) return;

        [application activateWithOptions:NSApplicationActivateIgnoringOtherApps];
    }
}
}