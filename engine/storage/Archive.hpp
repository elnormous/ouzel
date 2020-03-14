// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_STORAGE_ARCHIVE_HPP
#define OUZEL_STORAGE_ARCHIVE_HPP

#include <cstdint>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>
#include "storage/File.hpp"
#include "utils/Utils.hpp"

namespace ouzel
{
    namespace storage
    {
        class Archive final
        {
        public:
            Archive() = default;

            explicit Archive(const std::string& path):
                file{File(path, File::Mode::Read)}
            {
                constexpr std::uint32_t CENTRAL_DIRECTORY = 0x02014B50;
                constexpr std::uint32_t HEADER_SIGNATURE = 0x04034B50;

                for (;;)
                {
                    std::uint8_t signature[4];
                    file.read(signature, sizeof(signature), true);

                    if (decodeLittleEndian<std::uint32_t>(signature) == CENTRAL_DIRECTORY)
                        break;

                    if (decodeLittleEndian<std::uint32_t>(signature) != HEADER_SIGNATURE)
                        throw std::runtime_error("Bad signature");

                    std::uint8_t version[2];

                    file.read(version, sizeof(version), true);

                    std::uint16_t flags;

                    file.read(&flags, sizeof(flags), true);

                    std::uint16_t compression;
                    file.read(&compression, sizeof(compression), true);

                    if (compression != 0x00)
                        throw std::runtime_error("Unsupported compression");

                    file.seek(4, File::Seek::Current); // skip modification time
                    file.seek(4, File::Seek::Current); // skip CRC-32

                    std::uint8_t compressedSize[4];
                    file.read(compressedSize, sizeof(compressedSize), true);

                    std::uint8_t uncompressedSize[4];
                    file.read(uncompressedSize, sizeof(uncompressedSize), true);

                    std::uint8_t fileNameLength[2];
                    file.read(fileNameLength, sizeof(fileNameLength), true);

                    std::uint8_t extraFieldLength[2];
                    file.read(extraFieldLength, sizeof(extraFieldLength), true);

                    std::vector<char> name(decodeLittleEndian<std::uint16_t>(fileNameLength) + 1);

                    file.read(name.data(), decodeLittleEndian<std::uint16_t>(fileNameLength), true);

                    name[decodeLittleEndian<std::uint16_t>(fileNameLength)] = '\0';

                    Entry& entry = entries[name.data()];
                    entry.size = decodeLittleEndian<std::uint32_t>(uncompressedSize);

                    file.seek(decodeLittleEndian<std::uint16_t>(extraFieldLength), File::Seek::Current); // skip extra field

                    entry.offset = file.getOffset();

                    file.seek(static_cast<std::int32_t>(decodeLittleEndian<std::uint32_t>(uncompressedSize)),
                              File::Seek::Current); // skip uncompressed size
                }
            }

            std::vector<std::uint8_t> readFile(const std::string& filename) const
            {
                std::vector<std::uint8_t> data;

                auto i = entries.find(filename);

                if (i == entries.end())
                    throw std::runtime_error("File " + filename + " does not exist");

                file.seek(static_cast<std::int32_t>(i->second.offset), File::Seek::Begin);

                data.resize(i->second.size);

                file.read(data.data(), i->second.size, true);

                return data;
            }

            bool fileExists(const std::string& filename) const
            {
                return entries.find(filename) != entries.end();
            }

        private:
            File file;

            struct Entry final
            {
                std::uint32_t offset;
                std::uint32_t size;
            };

            std::map<std::string, Entry> entries;
        };
    } // namespace storage
} // namespace ouzel

#endif // OUZEL_STORAGE_ARCHIVE_HPP
