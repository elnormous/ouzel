#ifndef FLUXION_CORE_NATIVEWINDOWEM_HPP
#define FLUXION_CORE_NATIVEWINDOWEM_HPP

#include "../NativeWindow.hpp"

namespace fluxion::core::emscripten
{
    class NativeWindow final: public core::NativeWindow
    {
    public:
        NativeWindow(const math::Size<std::uint32_t, 2>& newSize,
                     bool newFullscreen,
                     const std::string& newTitle,
                     bool newHighDpi);

        void setSize(const math::Size<std::uint32_t, 2>& newSize);
        void setFullscreen(bool newFullscreen);

        void handleResize();
        void handleFullscreenChange(bool newFullscreen);
    
    private:
        void executeCommand(const Command& command) final;
    };
}

#endif // FLUXION_CORE_NATIVEWINDOWEM_HPP
