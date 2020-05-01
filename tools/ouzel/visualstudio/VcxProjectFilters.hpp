// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_VISUALSTUDIO_VCXPROJECTFILTERS_HPP
#define OUZEL_VISUALSTUDIO_VCXPROJECTFILTERS_HPP

namespace ouzel
{
    namespace visualstudio
    {
        class VcxProjectFilters final
        {
        public:
            VcxProjectFilters() = default;

            std::string encode() const
            {
                return std::string{};
            }
        };
    }
}

#endif // OUZEL_VISUALSTUDIO_VCXPROJECTFILTERS_HPP
