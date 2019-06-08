// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_CORE_TIMER_HPP
#define OUZEL_CORE_TIMER_HPP

namespace ouzel
{
    class Timer final
    {
    public:
        Timer() = default;
        ~Timer() = default;

        Timer(const Timer&) = delete;
        Timer& operator=(const Timer&) = delete;

        Timer(Timer&&) = delete;
        Timer& operator=(Timer&&) = delete;

        void update(float delta)
        {
            time += delta * scale;
        }

        inline float getTime() const { return time; }
        inline float getScale() const { return scale; }
        inline void setScale(float newScale) { scale = newScale; }

    private:
        float time = 0.0F;
        float scale = 1.0F;
    };
}

#endif // OUZEL_CORE_TIMER_HPP
