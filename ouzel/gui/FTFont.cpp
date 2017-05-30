#include "FTFont.h"

#include <ft2build.h>
#include FT_FREETYPE_H

ouzel::FTFont::FTFont() : BMFont()
{
}

ouzel::FTFont::FTFont(const std::string & filename, int pt):
BMFont(filename)
{

}
