// Ouzel by Elviss Strazdins

#ifndef OUZEL_PLATFORM_DISPATCH_DISPATCHSEMAPHORE_HPP
#define OUZEL_PLATFORM_DISPATCH_DISPATCHSEMAPHORE_HPP

#include <stdexcept>
#include <dispatch/dispatch.h>

namespace ouzel::platform::dispatch
{
    class DispatchSemaphore final
    {
    public:
        explicit DispatchSemaphore(intptr_t value):
            semaphore{dispatch_semaphore_create(value)}
        {
            if (!semaphore)
                throw std::runtime_error("Failed to create dispatch semaphore");
        }

        ~DispatchSemaphore()
        {
            if (semaphore) dispatch_release(semaphore);
        }

        DispatchSemaphore(DispatchSemaphore&& other) noexcept:
            semaphore{other.semaphore}
        {
            other.semaphore = nullptr;
        }

        DispatchSemaphore(const DispatchSemaphore& other):
            semaphore{other.semaphore}
        {
            if (semaphore) dispatch_retain(semaphore);
        }

        DispatchSemaphore& operator=(DispatchSemaphore&& other) noexcept
        {
            if (this == &other) return *this;
            if (semaphore) dispatch_release(semaphore);
            semaphore = other.semaphore;
            other.semaphore = nullptr;
            return *this;
        }

        DispatchSemaphore& operator=(const DispatchSemaphore& other) noexcept
        {
            if (this == &other) return *this;
            if (semaphore) dispatch_release(semaphore);
            semaphore = other.semaphore;
            if (semaphore) dispatch_retain(semaphore);
            return *this;
        }

        operator dispatch_semaphore_t() const noexcept
        {
            return semaphore;
        }

    private:
        dispatch_semaphore_t semaphore = nullptr;
    };
}

#endif // OUZEL_PLATFORM_DISPATCH_DISPATCHSEMAPHORE_HPP
