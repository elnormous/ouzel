// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_CORE_TIMER_HPP
#define OUZEL_CORE_TIMER_HPP

namespace ouzel
{
    class Timer final
    {
    public:
        Timer() noexcept = default;

        void update(const float delta) noexcept
        {
            time += delta * scale;
        }

        inline auto getTime() const noexcept { return time; }
        inline auto getScale() const noexcept { return scale; }
        inline void setScale(const float newScale) noexcept { scale = newScale; }

    private:
        float time = 0.0F;
        float scale = 1.0F;
    };
}

#endif // OUZEL_CORE_TIMER_HPP
