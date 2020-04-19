// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GUI_TTFONT_HPP
#define OUZEL_GUI_TTFONT_HPP

#include "../gui/Font.hpp"

struct stbtt_fontinfo;

namespace ouzel
{
    namespace gui
    {
        class TTFont final: public Font
        {
        public:
            TTFont() = default;
            TTFont(const std::vector<std::uint8_t>& newData, bool newMipmaps = true);

            RenderData getRenderData(const std::string& text,
                                     Color color,
                                     float fontSize,
                                     const Vector2F& anchor) const final;

            float getStringWidth(const std::string& text);

        private:
            std::unique_ptr<stbtt_fontinfo> font;
            std::vector<std::uint8_t> data;
            bool mipmaps = true;
        };
    } // namespace gui
} // namespace ouzel

#endif // OUZEL_GUI_TTFONT_HPP
