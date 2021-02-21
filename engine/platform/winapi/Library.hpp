// Ouzel by Elviss Strazdins

#ifndef OUZEL_PLATFORM_WINAPI_LIBRARY_HPP
#define OUZEL_PLATFORM_WINAPI_LIBRARY_HPP

#include <string>

#pragma push_macro("WIN32_LEAN_AND_MEAN")
#pragma push_macro("NOMINMAX")
#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#  define NOMINMAX
#endif
#include <Windows.h>
#pragma pop_macro("WIN32_LEAN_AND_MEAN")
#pragma pop_macro("NOMINMAX")

namespace ouzel::platform::winapi
{
    class Library final
    {
    public:
        Library() noexcept = default;
        Library(const std::string& filename) noexcept:
            handle(LoadLibraryA(filename.c_str()))
        {
        }

        Library(const Library&) = delete;
        Library& operator=(const Library&) = delete;

        Library(Library&& other) noexcept: handle(other.handle)
        {
            other.handle = nullptr;
        }

        Library& operator=(Library&& other) noexcept
        {
            if (this == &other) return *this;
            if (handle) FreeLibrary(handle);
            handle = other.handle;
            other.handle = nullptr;
            return *this;
        }

        ~Library()
        {
            if (handle) FreeLibrary(handle);
        }

        explicit operator bool() const noexcept
        {
            return handle != nullptr;
        }

        HMODULE get() const noexcept
        {
            return handle;
        }

    private:
        HMODULE handle = nullptr;
    };
}

#endif // OUZEL_PLATFORM_WINAPI_LIBRARY_HPP
