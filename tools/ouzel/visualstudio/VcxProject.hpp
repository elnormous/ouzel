// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_VISUALSTUDIO_VCXPROJECT_HPP
#define OUZEL_VISUALSTUDIO_VCXPROJECT_HPP

#include <random>

namespace ouzel
{
    namespace visualstudio
    {
        struct Guid final
        {
            std::uint32_t timeLow;
            std::uint16_t timeMid;
            std::uint16_t timeHiAndVersion;
            std::uint8_t clockSeqHiAndReserved;
            std::uint8_t clockSeqLow;
            std::uint8_t node[6];
        };

        inline Guid generateGuid()
        {
            static std::random_device rd;
            static std::mt19937_64 mt(rd());

            const std::uint64_t randomTime = mt();

            const std::uint32_t timeLow = ((randomTime >> 24) & 0x000000FF) |
                ((randomTime >> 8) & 0x0000FF00) |
                ((randomTime << 8) & 0x00FF0000) |
                ((randomTime << 24) & 0xFF000000);

            const std::uint16_t timeMid = static_cast<std::uint16_t>(((randomTime >> 40) & 0x00FF) |
                ((randomTime >> 24) & 0xFF00));

            const std::uint16_t timeHiAndVersion = static_cast<std::uint16_t>(((0x04 << 12) & 0xF000) |
                ((randomTime >> 56) & 0x00FF) |
                ((randomTime >> 40) & 0x0F00));

            const std::uint16_t clockSequence = static_cast<std::uint16_t>(mt() & 0x3FFF); // 14-bit random

            const std::uint8_t clockSeqHiAndReserved = static_cast<std::uint8_t>(0x80 | // bit 6 and 7
                ((clockSequence >> 8) & 0x3F));
            const std::uint8_t clockSeqLow = static_cast<std::uint8_t>(clockSequence & 0xFF);

            const std::uint64_t random = mt();

            return {
                timeLow,
                timeMid,
                timeHiAndVersion,
                clockSeqHiAndReserved,
                clockSeqLow,
                {
                    static_cast<std::uint8_t>(random >> 48),
                    static_cast<std::uint8_t>(random >> 40),
                    static_cast<std::uint8_t>(random >> 32),
                    static_cast<std::uint8_t>(random >> 24),
                    static_cast<std::uint8_t>(random >> 16),
                    static_cast<std::uint8_t>(random >> 0)
                }
            };
        }

        inline std::string toString(const Guid& guid)
        {
            constexpr char digits[] = "0123456789abcdef";

            return {
                digits[(guid.timeLow >> 28) & 0x0F],
                digits[(guid.timeLow >> 24) & 0x0F],
                digits[(guid.timeLow >> 20) & 0x0F],
                digits[(guid.timeLow >> 16) & 0x0F],
                digits[(guid.timeLow >> 12) & 0x0F],
                digits[(guid.timeLow >> 8) & 0x0F],
                digits[(guid.timeLow >> 4) & 0x0F],
                digits[(guid.timeLow >> 0) & 0x0F],
                '-',
                digits[(guid.timeMid >> 12) & 0x0F],
                digits[(guid.timeMid >> 8) & 0x0F],
                digits[(guid.timeMid >> 4) & 0x0F],
                digits[(guid.timeMid >> 0) & 0x0F],
                '-',
                digits[(guid.timeHiAndVersion >> 12) & 0x0F],
                digits[(guid.timeHiAndVersion >> 8) & 0x0F],
                digits[(guid.timeHiAndVersion >> 4) & 0x0F],
                digits[(guid.timeHiAndVersion >> 0) & 0x0F],
                '-',
                digits[(guid.clockSeqHiAndReserved >> 4) & 0x0F],
                digits[(guid.clockSeqHiAndReserved >> 0) & 0x0F],
                digits[(guid.clockSeqLow >> 4) & 0x0F],
                digits[(guid.clockSeqLow >> 0) & 0x0F],
                '-',
                digits[(guid.node[0] >> 4) & 0x0F],
                digits[(guid.node[0] >> 0) & 0x0F],
                digits[(guid.node[1] >> 4) & 0x0F],
                digits[(guid.node[1] >> 0) & 0x0F],
                digits[(guid.node[2] >> 4) & 0x0F],
                digits[(guid.node[2] >> 0) & 0x0F],
                digits[(guid.node[3] >> 4) & 0x0F],
                digits[(guid.node[3] >> 0) & 0x0F],
                digits[(guid.node[4] >> 4) & 0x0F],
                digits[(guid.node[4] >> 0) & 0x0F],
                digits[(guid.node[5] >> 4) & 0x0F],
                digits[(guid.node[5] >> 0) & 0x0F]
            };
        }

        class VcxProject final
        {
        public:
            VcxProject(const Guid& t,
                       const std::string& n,
                       const storage::Path& p,
                       const std::vector<Guid>& d):
                type{t}, name{n}, path{p}, dependencies(d) {}
            explicit VcxProject(const Guid& g,
                                const Guid& t,
                                const std::string& n,
                                const storage::Path& p,
                                const std::vector<Guid>& d):
                guid{g}, type{t}, name{n}, path{p}, dependencies(d) {}

            std::string encode() const
            {
                std::string result = "<?xml version=\"1.0\" encoding=\"utf - 8\"?>\n"
                    "<Project DefaultTargets=\"Build\" ToolsVersion=\"15.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\n";

                result += "</Project>";

                return result;
            }

            const Guid guid = generateGuid();
            const Guid type;
            const std::string name;
            const storage::Path path;
            const std::vector<Guid> dependencies;
        };
    }
}

#endif // OUZEL_VISUALSTUDIO_VCXPROJECT_HPP
