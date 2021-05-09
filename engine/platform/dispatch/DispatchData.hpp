// Ouzel by Elviss Strazdins

#ifndef OUZEL_PLATFORM_DISPATCH_DISPATCHDATA_HPP
#define OUZEL_PLATFORM_DISPATCH_DISPATCHDATA_HPP

#include <stdexcept>
#include <dispatch/data.h>

namespace ouzel::platform::dispatch
{
    class DispatchData final
    {
    public:
        explicit DispatchData(const void* buffer,
                              size_t size,
                              dispatch_queue_t queue,
                              dispatch_block_t destructor):
            data{dispatch_data_create(buffer, size, queue, destructor)}
        {
            if (!data)
                throw std::runtime_error("Failed to create dispatch data");
        }

        ~DispatchData()
        {
            if (data) dispatch_release(data);
        }

        DispatchData(DispatchData&& other) noexcept:
            data{other.data}
        {
            other.data = nullptr;
        }

        DispatchData(const DispatchData& other):
            data{other.data}
        {
            if (data) dispatch_retain(data);
        }

        DispatchData& operator=(DispatchData&& other) noexcept
        {
            if (this == &other) return *this;
            if (data) dispatch_release(data);
            data = other.data;
            other.data = nullptr;
            return *this;
        }

        DispatchData& operator=(const DispatchData& other) noexcept
        {
            if (this == &other) return *this;
            if (data) dispatch_release(data);
            data = other.data;
            if (data) dispatch_retain(data);
            return *this;
        }

        operator dispatch_data_t() const noexcept
        {
            return data;
        }

    private:
        dispatch_data_t data = nullptr;
    };
}

#endif // OUZEL_PLATFORM_DISPATCH_DISPATCHDATA_HPP
