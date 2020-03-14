// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_STORAGE_ARCHIVE_HPP
#define OUZEL_STORAGE_ARCHIVE_HPP

#include <cstdint>
#include <fstream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>
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
                file{path, std::ios::binary}
            {
                constexpr std::uint32_t CENTRAL_DIRECTORY = 0x02014B50;
                constexpr std::uint32_t HEADER_SIGNATURE = 0x04034B50;

                for (;;)
                {
                    std::uint8_t signature[4];
                    file.read(reinterpret_cast<char*>(signature), sizeof(signature));

                    if (decodeLittleEndian<std::uint32_t>(signature) == CENTRAL_DIRECTORY)
                        break;

                    if (decodeLittleEndian<std::uint32_t>(signature) != HEADER_SIGNATURE)
                        throw std::runtime_error("Bad signature");

                    std::uint8_t version[2];

                    file.read(reinterpret_cast<char*>(version), sizeof(version));

                    std::uint16_t flags;

                    file.read(reinterpret_cast<char*>(&flags), sizeof(flags));

                    std::uint16_t compression;
                    file.read(reinterpret_cast<char*>(&compression), sizeof(compression));

                    if (compression != 0x00)
                        throw std::runtime_error("Unsupported compression");

                    file.seekg(4, std::ios::cur); // skip modification time
                    file.seekg(4, std::ios::cur); // skip CRC-32

                    std::uint8_t compressedSize[4];
                    file.read(reinterpret_cast<char*>(compressedSize), sizeof(compressedSize));

                    std::uint8_t uncompressedSize[4];
                    file.read(reinterpret_cast<char*>(uncompressedSize), sizeof(uncompressedSize));

                    std::uint8_t fileNameLength[2];
                    file.read(reinterpret_cast<char*>(fileNameLength), sizeof(fileNameLength));

                    std::uint8_t extraFieldLength[2];
                    file.read(reinterpret_cast<char*>(extraFieldLength), sizeof(extraFieldLength));

                    std::vector<char> name(decodeLittleEndian<std::uint16_t>(fileNameLength) + 1);

                    file.read(name.data(), decodeLittleEndian<std::uint16_t>(fileNameLength));

                    name[decodeLittleEndian<std::uint16_t>(fileNameLength)] = '\0';

                    Entry& entry = entries[name.data()];
                    entry.size = decodeLittleEndian<std::uint32_t>(uncompressedSize);

                    file.seekg(decodeLittleEndian<std::uint16_t>(extraFieldLength), std::ios::cur); // skip extra field

                    entry.offset = file.tellg();

                    file.seekg(static_cast<std::streamoff>(decodeLittleEndian<std::uint32_t>(uncompressedSize)),
                               std::ios::cur); // skip uncompressed size
                }
            }

            std::vector<std::uint8_t> readFile(const std::string& filename)
            {
                std::vector<std::uint8_t> data;

                auto i = entries.find(filename);

                if (i == entries.end())
                    throw std::runtime_error("File " + filename + " does not exist");

                file.seekg(i->second.offset, std::ios::beg);

                data.resize(i->second.size);

                file.read(reinterpret_cast<char*>(data.data()), i->second.size);

                return data;
            }

            bool fileExists(const std::string& filename) const
            {
                return entries.find(filename) != entries.end();
            }

        private:
            std::ifstream file;

            struct Entry final
            {
                std::streamoff offset;
                std::size_t size;
            };

            std::map<std::string, Entry> entries;
        };
    } // namespace storage
} // namespace ouzel

#endif // OUZEL_STORAGE_ARCHIVE_HPP
