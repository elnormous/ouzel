// Ouzel by Elviss Strazdins

#ifndef OUZEL_PLATFORM_DISPATCH_SEMAPHORE_HPP
#define OUZEL_PLATFORM_DISPATCH_SEMAPHORE_HPP

#include <stdexcept>
#include <dispatch/dispatch.h>

namespace ouzel::platform::dispatch
{
    class Semaphore final
    {
    public:
        explicit Semaphore(intptr_t value):
            semaphore{dispatch_semaphore_create(value)}
        {
            if (!semaphore)
                throw std::runtime_error{"Failed to create dispatch semaphore"};
        }

        ~Semaphore()
        {
            if (semaphore) dispatch_release(semaphore);
        }

        Semaphore(Semaphore&& other) noexcept:
            semaphore{other.semaphore}
        {
            other.semaphore = nullptr;
        }

        Semaphore(const Semaphore& other) noexcept:
            semaphore{other.semaphore}
        {
            if (semaphore) dispatch_retain(semaphore);
        }

        Semaphore& operator=(Semaphore&& other) noexcept
        {
            if (&other == this) return *this;
            if (semaphore) dispatch_release(semaphore);
            semaphore = other.semaphore;
            other.semaphore = nullptr;
            return *this;
        }

        Semaphore& operator=(const Semaphore& other) noexcept
        {
            if (&other == this) return *this;
            if (other.semaphore) dispatch_retain(other.semaphore);
            if (semaphore) dispatch_release(semaphore);
            semaphore = other.semaphore;
            return *this;
        }

        void wait(dispatch_time_t timeout) noexcept
        {
            if (semaphore) dispatch_semaphore_wait(semaphore, timeout);
        }

        void signal() noexcept
        {
            if (semaphore) dispatch_semaphore_signal(semaphore);
        }

    private:
        dispatch_semaphore_t semaphore = nullptr;
    };
}

#endif // OUZEL_PLATFORM_DISPATCH_SEMAPHORE_HPP
