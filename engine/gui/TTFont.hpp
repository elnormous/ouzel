// Ouzel by Elviss Strazdins

#ifndef OUZEL_GUI_TTFONT_HPP
#define OUZEL_GUI_TTFONT_HPP

#include "../gui/Font.hpp"

struct stbtt_fontinfo;

namespace ouzel::gui
{
    class TTFont final: public Font
    {
    public:
        TTFont() = default;
        TTFont(const std::vector<std::byte>& newData, bool newMipmaps = true);

        RenderData getRenderData(const std::string& text,
                                 Color color,
                                 float fontSize,
                                 const Vector2F& anchor) const final;

        float getStringWidth(const std::string& text);

    private:
        std::unique_ptr<stbtt_fontinfo> font;
        std::vector<std::byte> data;
        bool mipmaps = true;
    };
}

#endif // OUZEL_GUI_TTFONT_HPP
