// Ouzel by Elviss Strazdins

#ifndef OUZEL_THREAD_CHANNEL_HPP
#define OUZEL_THREAD_CHANNEL_HPP

#include <condition_variable>
#include <memory>
#include <mutex>
#include <vector>

namespace ouzel::thread
{
    template <class Type>
    class ChannelContainer final
    {
    public:
        ChannelContainer() = default;
        ~ChannelContainer()
        {
            std::unique_lock lock(queueMutex);
            closed = true;
            lock.unlock();
            queueCondition.notify_all();
        }

        ChannelContainer(const ChannelContainer&) = delete;
        ChannelContainer& operator=(const ChannelContainer&) = delete;
        ChannelContainer(ChannelContainer&&) = delete;
        ChannelContainer& operator=(ChannelContainer&&) = delete;

        template <class T>
        void send(T&& entry)
        {
            std::unique_lock lock(queueMutex);
            if (!closed) queue.push(std::forward<T>(entry));
            lock.unlock();
            queueCondition.notify_all();
        }

        void close()
        {
            std::unique_lock lock(queueMutex);
            closed = true;
            lock.unlock();
            queueCondition.notify_all();
        }

        std::unique_ptr<Type> next() const
        {
            std::unique_lock lock(queueMutex);
            while (!closed && queue.empty()) queueCondition.wait(lock);

            if (!queue.empty())
            {
                auto result = std::make_unique<Type>(std::move(queue.front()));
                queue.pop();
                return result;
            }
            else
                return std::unique_ptr<Type>{};
        }

    private:
        bool closed = false;
        mutable std::queue<Type> queue;
        mutable std::mutex queueMutex;
        mutable std::condition_variable queueCondition;
    };

    template <class Type>
    class ChannelIterator final
    {
    public:
        explicit ChannelIterator(const ChannelContainer<Type>& c) noexcept:
            container{c} {}
        ChannelIterator(const ChannelContainer<Type>& c, std::unique_ptr<Type> p) noexcept:
            container{c}, ptr{std::move(p)} {}

        operator bool() const noexcept { return ptr != nullptr; }

        bool operator==(const ChannelIterator& i) const noexcept { return ptr == i.ptr; }
        bool operator!=(const ChannelIterator& i) const noexcept { return ptr != i.ptr; }

        Type& operator*() noexcept { return *ptr; }
        const Type& operator*() const noexcept { return *ptr; }
        Type* operator->() noexcept { return ptr; }
        const Type* operator->() const noexcept { return ptr; }

        ChannelIterator& operator++() noexcept
        {
            ptr = container.next();
            return *this;
        }

    private:
        const ChannelContainer<Type>& container;
        std::unique_ptr<Type> ptr;
    };

    template <class Type>
    class Channel final
    {
    public:
        using Iterator = ChannelIterator<Type>;
        using ConstIterator = ChannelIterator<const Type>;

        Channel() = default;

        Iterator begin() noexcept { return Iterator{container, container.next()}; }
        ConstIterator begin() const noexcept { return Iterator{container, container.next()}; }
        Iterator end() noexcept { return Iterator{container}; }
        ConstIterator end() const noexcept { return Iterator{container}; }

        template <class T>
        void send(T&& entry)
        {
            container.send(std::forward<T>(entry));
        }

        void close()
        {
            container.close();
        }

    private:
        ChannelContainer<Type> container;
    };
}

#endif // OUZEL_THREAD_CHANNEL_HPP
