#ifndef FLUXION_CORE_TIMER_HPP
#define FLUXION_CORE_TIMER_HPP

namespace fluxion::core
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

#endif // FLUXION_CORE_TIMER_HPP
