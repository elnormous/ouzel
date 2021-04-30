// Ouzel by Elviss Strazdins

#ifndef OUZEL_GUI_BMFONT_HPP
#define OUZEL_GUI_BMFONT_HPP

#include "Font.hpp"

namespace ouzel::gui
{
    class BMFont final: public Font
    {
    public:
        BMFont() = default;
        explicit BMFont(const std::vector<std::byte>& data);

        RenderData getRenderData(const std::string& text,
                                 Color color,
                                 float fontSize,
                                 const Vector<float, 2>& anchor) const override;

        float getStringWidth(const std::string& text) const noexcept;

    private:
        std::int16_t getKerningPair(char32_t, char32_t) const;

        struct CharDescriptor final
        {
            std::int16_t x = 0;
            std::int16_t y = 0;
            std::int16_t width = 0;
            std::int16_t height = 0;
            std::int16_t xOffset = 0;
            std::int16_t yOffset = 0;
            std::int16_t xAdvance = 0;
            std::int16_t page = 0;
        };

        std::uint16_t lineHeight = 0;
        std::uint16_t base = 0;
        std::uint16_t width = 0;
        std::uint16_t height = 0;
        std::uint16_t pages = 0;
        std::uint16_t outline = 0;
        std::uint16_t kernCount = 0;
        std::unordered_map<char32_t, CharDescriptor> chars;
        std::map<std::pair<char32_t, char32_t>, std::int16_t> kern;
        std::shared_ptr<graphics::Texture> fontTexture;
    };
}

#endif // OUZEL_GUI_BMFONT_HPP
