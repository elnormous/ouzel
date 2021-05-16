// Ouzel by Elviss Strazdins

#ifndef OUZEL_THREAD_SEMAPHORE_HPP
#define OUZEL_THREAD_SEMAPHORE_HPP

#include <system_error>
#ifdef _WIN32
#  pragma push_macro("WIN32_LEAN_AND_MEAN")
#  pragma push_macro("NOMINMAX")
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  include <Windows.h>
#  pragma pop_macro("WIN32_LEAN_AND_MEAN")
#  pragma pop_macro("NOMINMAX")
#elif defined(__APPLE__)
#  include <dispatch/dispatch.h>
#else
#  include <semaphore.h>
#endif

namespace ouzel::thread
{
    class Semaphore final
    {
    public:
        Semaphore(std::ptrdiff_t initValue = 0)
#ifdef _MSC_VER
            : semaphore{CreateSemaphore(nullptr, static_cast<LONG>(initValue), 0x7FFFFFFF, nullptr)}
#elif defined(__APPLE__)
            : semaphore{dispatch_semaphore_create(static_cast<ptrdiff_t>(initValue))}
#endif
        {
#ifdef _MSC_VER
            if (semaphore == INVALID_HANDLE_VALUE)
                throw std::system_error(GetLastError(), std::system_category(), "Failed to create semaphore");
#elif defined(__APPLE__)
            if (!semaphore)
                throw std::runtime_error("Failed to create semaphore");
#else
            if (sem_init(&semaphore, 0, static_cast<unsigned int>(initValue)) == -1)
                throw std::system_error(errno, std::system_category(), "Failed to open semaphore");
#endif
        }

        ~Semaphore()
        {
#ifdef _MSC_VER
            if (semaphore != INVALID_HANDLE_VALUE)
                CloseHandle(semaphore);
#elif defined(__APPLE__)
            if (semaphore)
                dispatch_release(semaphore);
#else
            if (semaphore != -1)
                sem_destroy(&semaphore);
#endif
        }

        Semaphore(const Semaphore&) = delete;
        Semaphore(Semaphore&& other) noexcept:
            semaphore{other.semaphore}
        {
#ifdef _MSC_VER
            other.semaphore = INVALID_HANDLE_VALUE;
#elif defined(__APPLE__)
            other.semaphore = nullptr;
#else
            other.semaphore = -1;
#endif
        }

        Semaphore& operator=(const Semaphore&) = delete;
        Semaphore& operator=(Semaphore&& other) noexcept
        {
            if (&other == this) return *this;

            semaphore = other.semaphore;

#ifdef _MSC_VER
            other.semaphore = INVALID_HANDLE_VALUE;
#elif defined(__APPLE__)
            other.semaphore = nullptr;
#else
            other.semaphore = -1;
#endif

            return *this;
        }

        void acquire()
        {
#ifdef _MSC_VER
            if (WaitForSingleObject(semaphore, INFINITE) == WAIT_FAILED)
                throw std::system_error(GetLastError(), std::system_category(), "Failed to wait for semaphore");
#elif defined(__APPLE__)
            if (dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER) != 0)
                throw std::runtime_error("Failed to wait for semaphore");
#else
            if (sem_wait(&semaphore) == -1)
                throw std::system_error(errno, std::system_category(), "Failed to wait for semaphore");
#endif
        }

        void release(std::ptrdiff_t count)
        {
#ifdef _MSC_VER
            if (!ReleaseSemaphore(semaphore, static_cast<LONG>(count), nullptr))
                throw std::system_error(GetLastError(), std::system_category(), "Failed to release semaphore");
#elif defined(__APPLE__)
            while (count-- > 0)
                if (dispatch_semaphore_signal(semaphore) != 0)
                    throw std::runtime_error("Failed to signal semaphore");
#else
            while (count-- > 0)
                if (sem_post(&semaphore) == -1)
                    throw std::system_error(errno, std::system_category(), "Failed to post semaphore");
#endif
        }

    private:
#ifdef _MSC_VER
        HANDLE semaphore = INVALID_HANDLE_VALUE;
#elif defined(__APPLE__)
        dispatch_semaphore_t semaphore = nullptr;
#else
        sem_t semaphore = -1;
#endif
    };
}

#endif // OUZEL_THREAD_SEMAPHORE_HPP
