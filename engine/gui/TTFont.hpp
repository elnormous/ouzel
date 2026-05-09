#ifndef FLUXION_GUI_TTFONT_HPP
#define FLUXION_GUI_TTFONT_HPP

#include "../gui/Font.hpp"

struct stbtt_fontinfo;

namespace fluxion::gui
{
    class TTFont final: public Font
    {
    public:
        TTFont() = default;
        TTFont(const std::vector<std::byte>& newData, bool newMipmaps = true);

        RenderData getRenderData(const std::string_view& text,
                                 math::Color color,
                                 float fontSize,
                                 const math::Vector<float, 2>& anchor) const override;

    private:
        std::unique_ptr<stbtt_fontinfo> font;
        std::vector<std::byte> data;
        bool mipmaps = true;
    };
}

#endif // FLUXION_GUI_TTFONT_HPP
