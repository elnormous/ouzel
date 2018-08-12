// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Archive.hpp"
#include "FileSystem.hpp"
#include "utils/Errors.hpp"
#include "utils/Utils.hpp"

namespace ouzel
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

            if (decodeUInt32Little(&signature) == 0x02014b50) // central directory
                break;

            if (decodeUInt32Little(&signature) != 0x04034b50)
                throw ParseError("Bad signature");

            uint8_t version[2];

            file.read(version, sizeof(version), true);

            uint16_t flags;

            file.read(&flags, sizeof(flags), true);

            uint16_t compression;
            file.read(&compression, sizeof(compression), true);

            if (compression != 0x00)
                throw ParseError("Unsupported compression");

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

            std::vector<char> name(decodeUInt16Little(&fileNameLength) + 1);

            file.read(name.data(), decodeUInt16Little(&fileNameLength), true);

            name[decodeUInt16Little(&fileNameLength)] = '\0';

            Entry& entry = entries[name.data()];
            entry.size = decodeUInt32Little(&uncompressedSize);

            file.seek(decodeUInt16Little(&extraFieldLength), File::CURRENT); // skip extra field

            entry.offset = file.getOffset();

            file.seek(decodeInt32Little(&uncompressedSize), File::CURRENT); // skip uncompressed size
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
            throw FileError("File " + filename + " does not exist");

        file.seek(static_cast<int32_t>(i->second.offset), File::BEGIN);

        data.resize(i->second.size);

        file.read(data.data(), i->second.size, true);

        return data;
    }

    bool Archive::fileExists(const std::string& filename) const
    {
        return entries.find(filename) != entries.end();
    }
}
