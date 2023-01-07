// Ouzel by Elviss Strazdins

#ifndef OUZEL_PLATFORM_DISPATCH_DATA_HPP
#define OUZEL_PLATFORM_DISPATCH_DATA_HPP

#include <stdexcept>
#include <dispatch/data.h>

namespace ouzel::platform::dispatch
{
    class Data final
    {
    public:
        explicit Data(const void* buffer,
                      const size_t size,
                      const dispatch_queue_t queue,
                      const dispatch_block_t destructor):
            data{dispatch_data_create(buffer, size, queue, destructor)}
        {
            if (!data)
                throw std::runtime_error{"Failed to create dispatch data"};
        }

        ~Data()
        {
            if (data) dispatch_release(data);
        }

        Data(Data&& other) noexcept:
            data{other.data}
        {
            other.data = nullptr;
        }

        Data(const Data& other) noexcept:
            data{other.data}
        {
            if (data) dispatch_retain(data);
        }

        Data& operator=(Data&& other) noexcept
        {
            if (&other == this) return *this;
            if (data) dispatch_release(data);
            data = other.data;
            other.data = nullptr;
            return *this;
        }

        Data& operator=(const Data& other) noexcept
        {
            if (&other == this) return *this;
            if (other.data) dispatch_retain(other.data);
            if (data) dispatch_release(data);
            data = other.data;
            return *this;
        }

        operator dispatch_data_t() const noexcept
        {
            return data;
        }

        auto get() const noexcept
        {
            return data;
        }

    private:
        dispatch_data_t data = nullptr;
    };
}

#endif // OUZEL_PLATFORM_DISPATCH_DATA_HPP
