// Ouzel by Elviss Strazdins

#ifndef OUZEL_CORE_TIMER_HPP
#define OUZEL_CORE_TIMER_HPP

namespace ouzel::core
{
    class Timer final
    {
    public:
        Timer() noexcept = default;

        void update(const float delta) noexcept
        {
            time += delta * scale;
        }

        auto getTime() const noexcept { return time; }
        auto getScale() const noexcept { return scale; }
        void setScale(const float newScale) noexcept { scale = newScale; }

    private:
        float time = 0.0F;
        float scale = 1.0F;
    };
}

#endif // OUZEL_CORE_TIMER_HPP
