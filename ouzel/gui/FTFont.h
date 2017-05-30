#pragma once


#include "BMFont.h"

namespace ouzel
{

	class FTFont : public BMFont
	{
	public:
		FTFont();
		FTFont(const std::string& filename, int pt);
	};
}