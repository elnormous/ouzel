#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H

#include "BMFont.h"

namespace ouzel
{

	class FFTFont 
	{
	public:
		FFTFont(std::string& filename, int pt);
	};
}