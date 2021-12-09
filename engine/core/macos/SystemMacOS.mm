// Ouzel by Elviss Strazdins

#include <cstdlib>
#include "SystemMacOS.hpp"
#include "EngineMacOS.hpp"
#include "../../platform/foundation/AutoreleasePool.hpp"
#include "../../utils/Log.hpp"

ouzel::core::macos::System* systemPointer;

@interface AppDelegate: NSObject<NSApplicationDelegate>
@end

@implementation AppDelegate

- (void)applicationWillFinishLaunching:(__unused NSNotification*)notification
{
    if (systemPointer) systemPointer->start();
}

- (void)applicationDidFinishLaunching:(__unused NSNotification*)notification
{
    if (ouzel::engine) ouzel::engine->start();
}

- (void)applicationWillTerminate:(__unused NSNotification*)notification
{
    if (ouzel::engine) ouzel::engine->exit();
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(__unused NSApplication*)sender
{
    return YES;
}

- (BOOL)application:(__unused NSApplication*)sender openFile:(NSString*)filename
{
    if (ouzel::engine)
    {
        auto event = std::make_unique<ouzel::SystemEvent>();
        event->type = ouzel::Event::Type::openFile;
        event->filename = [filename cStringUsingEncoding:NSUTF8StringEncoding];
        ouzel::engine->getEventDispatcher().postEvent(std::move(event));
    }

    return YES;
}

- (void)applicationDidBecomeActive:(__unused NSNotification*)notification
{
    //if (ouzel::engine) ouzel::engine->resume();
}

- (void)applicationDidResignActive:(__unused NSNotification*)notification
{
    //if (ouzel::engine) ouzel::engine->pause();
}
@end

int main(int argc, char* argv[])
{
    try
    {
        ouzel::core::macos::System system{argc, argv};
        systemPointer = &system;
        system.run();
        return EXIT_SUCCESS;
    }
    catch (const std::exception& e)
    {
        ouzel::log(ouzel::Log::Level::error) << e.what();
        return EXIT_FAILURE;
    }
}

namespace ouzel::core::macos
{
    namespace
    {
        std::vector<std::string> parseArgs(int argc, char* argv[])
        {
            std::vector<std::string> result;
            for (int i = 0; i < argc; ++i)
                result.push_back(argv[i]);
            return result;
        }
    }

    System::System(int argc, char* argv[]):
        core::System{parseArgs(argc, argv)},
        application{NSApplication.sharedApplication}
    {
    }

    void System::run()
    {
        ouzel::platform::foundation::AutoreleasePool autoreleasePool;

        [application activateIgnoringOtherApps:YES];
        application.delegate = [[[AppDelegate alloc] init] autorelease];

        NSMenu* mainMenu = [[[NSMenu alloc] initWithTitle:@"Main Menu"] autorelease];

        // Apple menu
        NSMenuItem* mainMenuItem = [mainMenu addItemWithTitle:@"Apple"
                                                       action:nil
                                                keyEquivalent:@""];

        NSMenu* applicationMenu = [[[NSMenu alloc] init] autorelease];
        mainMenuItem.submenu = applicationMenu;

        NSMenuItem* servicesItem = [applicationMenu addItemWithTitle:NSLocalizedString(@"Services", nil)
                                                              action:nil
                                                       keyEquivalent:@""];

        NSMenu* servicesMenu = [[[NSMenu alloc] init] autorelease];
        servicesItem.submenu = servicesMenu;
        application.servicesMenu = servicesMenu;

        [applicationMenu addItem:[NSMenuItem separatorItem]];

        NSString* bundleName = NSBundle.mainBundle.infoDictionary[@"CFBundleDisplayName"];
        if (!bundleName)
            bundleName = NSBundle.mainBundle.infoDictionary[@"CFBundleName"];

        NSMenuItem* hideItem = [applicationMenu addItemWithTitle:[NSString stringWithFormat:@"%@ %@", NSLocalizedString(@"Hide", nil), bundleName]
                                                          action:@selector(hide:)
                                                   keyEquivalent:@"h"];
        hideItem.target = application;

        NSMenuItem* hideOthersItem = [applicationMenu addItemWithTitle:NSLocalizedString(@"Hide Others", nil)
                                                                action:@selector(hideOtherApplications:)
                                                         keyEquivalent:@"h"];
        hideOthersItem.keyEquivalentModifierMask = NSEventModifierFlagOption | NSEventModifierFlagCommand;
        hideOthersItem.target = application;

        NSMenuItem* showAllItem = [applicationMenu addItemWithTitle:NSLocalizedString(@"Show All", nil)
                                                             action:@selector(unhideAllApplications:)
                                                      keyEquivalent:@""];
        showAllItem.target = application;

        [applicationMenu addItem:[NSMenuItem separatorItem]];

        NSMenuItem* quitItem = [applicationMenu addItemWithTitle:[NSString stringWithFormat:@"%@ %@", NSLocalizedString(@"Quit", nil), bundleName]
                                                          action:@selector(terminate:)
                                                   keyEquivalent:@"q"];
        quitItem.target = application;

        // View menu
        NSMenuItem* viewItem = [mainMenu addItemWithTitle:NSLocalizedString(@"View", nil)
                                                   action:nil
                                            keyEquivalent:@""];

        NSMenu* viewMenu = [[[NSMenu alloc] initWithTitle:NSLocalizedString(@"View", nil)] autorelease];
        viewItem.submenu = viewMenu;

        // Window menu
        NSMenuItem* windowsItem = [mainMenu addItemWithTitle:NSLocalizedString(@"Window", nil)
                                                      action:nil
                                               keyEquivalent:@""];

        NSMenu* windowsMenu = [[[NSMenu alloc] initWithTitle:NSLocalizedString(@"Window", nil)] autorelease];
        windowsItem.submenu = windowsMenu;
        application.windowsMenu = windowsMenu;

        application.mainMenu = mainMenu;

        [application run];
    }

    void System::start()
    {
        engine = std::make_unique<Engine>(getArgs());
    }
}
