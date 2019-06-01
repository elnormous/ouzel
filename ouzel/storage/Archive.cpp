// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <stdexcept>
#include "Archive.hpp"
#include "FileSystem.hpp"
#include "utils/Utils.hpp"

namespace ouzel
{
    namespace storage
    {
        Archive::Archive(FileSystem& initFileSystem, const std::string& filename):
            fileSystem(initFileSystem)
        {
            fileSystem.addArchive(this);

            file = File(fileSystem.getPath(filename), File::READ);

            for (;;)
            {
                uint32_t signature;

                file.read(&signature, sizeof(signature), true);

                if (decodeLittleEndian<uint32_t>(&signature) == 0x02014B50) // central directory
                    break;

                if (decodeLittleEndian<uint32_t>(&signature) != 0x04034B50)
                    throw std::runtime_error("Bad signature");

                uint8_t version[2];

                file.read(version, sizeof(version), true);

                uint16_t flags;

                file.read(&flags, sizeof(flags), true);

                uint16_t compression;
                file.read(&compression, sizeof(compression), true);

                if (compression != 0x00)
                    throw std::runtime_error("Unsupported compression");

                file.seek(4, File::CURRENT); // skip modification time
                file.seek(4, File::CURRENT); // skip CRC-32

                uint32_t compressedSize;
                file.read(&compressedSize, sizeof(compressedSize), true);

                uint32_t uncompressedSize;
                file.read(&uncompressedSize, sizeof(uncompressedSize), true);

                uint16_t fileNameLength;
                file.read(&fileNameLength, sizeof(fileNameLength), true);

                uint16_t extraFieldLength;
                file.read(&extraFieldLength, sizeof(extraFieldLength), true);

                std::vector<char> name(decodeLittleEndian<uint16_t>(&fileNameLength) + 1);

                file.read(name.data(), decodeLittleEndian<uint16_t>(&fileNameLength), true);

                name[decodeLittleEndian<uint16_t>(&fileNameLength)] = '\0';

                Entry& entry = entries[name.data()];
                entry.size = decodeLittleEndian<uint32_t>(&uncompressedSize);

                file.seek(decodeLittleEndian<uint16_t>(&extraFieldLength), File::CURRENT); // skip extra field

                entry.offset = file.getOffset();

                file.seek(static_cast<int32_t>(decodeLittleEndian<uint32_t>(&uncompressedSize)), File::CURRENT); // skip uncompressed size
            }
        }

        Archive::~Archive()
        {
            fileSystem.removeArchive(this);
        }

        std::vector<uint8_t> Archive::readFile(const std::string& filename) const
        {
            std::vector<uint8_t> data;

            auto i = entries.find(filename);

            if (i == entries.end())
                throw std::runtime_error("File " + filename + " does not exist");

            file.seek(static_cast<int32_t>(i->second.offset), File::BEGIN);

            data.resize(i->second.size);

            file.read(data.data(), i->second.size, true);

            return data;
        }

        bool Archive::fileExists(const std::string& filename) const
        {
            return entries.find(filename) != entries.end();
        }
    } // namespace storage
} // namespace ouzel
