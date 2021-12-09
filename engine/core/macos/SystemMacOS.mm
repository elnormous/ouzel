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

- (void)handleQuit:(id)sender
{
    [[NSApplication sharedApplication] terminate:sender];
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
        application{[NSApplication sharedApplication]}
    {
    }

    void System::run()
    {
        ouzel::platform::foundation::AutoreleasePool autoreleasePool;

        [application activateIgnoringOtherApps:YES];
        [application setDelegate:[[[AppDelegate alloc] init] autorelease]];

        NSMenu* mainMenu = [[[NSMenu alloc] initWithTitle:@"Main Menu"] autorelease];

        NSMenuItem* mainMenuItem = [[[NSMenuItem alloc] init] autorelease];
        [mainMenu addItem:mainMenuItem];

        NSMenu* subMenu = [[[NSMenu alloc] init] autorelease];
        [mainMenuItem setSubmenu:subMenu];

        NSMenuItem* quitItem = [[[NSMenuItem alloc] initWithTitle:NSLocalizedString(@"Quit", nil) action:@selector(handleQuit:) keyEquivalent:@"q"] autorelease];
        [quitItem setTarget:[application delegate]];
        [subMenu addItem:quitItem];

        application.mainMenu = mainMenu;

        [application run];
    }

    void System::start()
    {
        engine = std::make_unique<Engine>(getArgs());
    }
}
