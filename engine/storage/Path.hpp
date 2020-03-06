// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_STORAGE_PATH_HPP
#define OUZEL_STORAGE_PATH_HPP

#include <string>

namespace ouzel
{
    namespace storage
    {
        class Path final
        {
        public:
#if defined(_WIN32)
                static constexpr char DIRECTORY_SEPARATOR = '\\';
#elif defined(__unix__) || defined(__APPLE__)
                static constexpr char DIRECTORY_SEPARATOR = '/';
#endif

            Path() = default;
            Path(const std::string& path)
            {
                // TODO: normalize
            }

        };
    } // namespace storage
} // namespace ouzel

#endif
