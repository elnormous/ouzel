// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Archive.hpp"
#include "FileSystem.hpp"
#include "core/Engine.hpp"
#include "utils/Log.hpp"
#include "utils/Utils.hpp"

namespace ouzel
{
    Archive::Archive(const std::string& filename)
    {
        open(filename);
    }

    Archive::~Archive()
    {
        if (fileSystem) fileSystem->removeArchive(this);
    }

    bool Archive::open(const std::string& filename)
    {
        if (!file.open(engine->getFileSystem()->getPath(filename, false), File::READ))
        {
            Log(Log::Level::ERR) << "Failed to open file " << filename;
            return false;
        }

        for (;;)
        {
            uint32_t signature;

            if (!file.read(&signature, sizeof(signature)))
            {
                Log(Log::Level::ERR) << "Failed to read signature";
                return false;
            }

            if (decodeUInt32Little(&signature) == 0x02014b50) // central directory
                break;

            if (decodeUInt32Little(&signature) != 0x04034b50)
            {
                Log(Log::Level::ERR) << "Bad signature";
                return false;
            }

            uint8_t version[2];

            if (!file.read(version, sizeof(version)))
            {
                Log(Log::Level::ERR) << "Failed to read version";
                return false;
            }

            uint16_t flags;

            if (!file.read(&flags, sizeof(flags)))
            {
                Log(Log::Level::ERR) << "Failed to read flags";
                return false;
            }

            uint16_t compression;
            if (!file.read(&compression, sizeof(compression)))
            {
                Log(Log::Level::ERR) << "Failed to read compression";
                return false;
            }

            if (compression != 0x00)
            {
                Log(Log::Level::ERR) << "Unsupported compression";
                return false;
            }

            if (!file.seek(4, File::CURRENT))
            {
                Log(Log::Level::ERR) << "Failed to skip file modification time";
                return false;
            }

            if (!file.seek(4, File::CURRENT))
            {
                Log(Log::Level::ERR) << "Failed to skip CRC-32";
                return false;
            }

            uint32_t compressedSize;
            if (!file.read(&compressedSize, sizeof(compressedSize)))
            {
                Log(Log::Level::ERR) << "Failed to read compressed size";
                return false;
            }

            uint32_t uncompressedSize;
            if (!file.read(&uncompressedSize, sizeof(uncompressedSize)))
            {
                Log(Log::Level::ERR) << "Failed to read compressed size";
                return false;
            }

            uint16_t fileNameLength;
            if (!file.read(&fileNameLength, sizeof(fileNameLength)))
            {
                Log(Log::Level::ERR) << "Failed to read file name length";
                return false;
            }

            uint16_t extraFieldLength;
            if (!file.read(&extraFieldLength, sizeof(extraFieldLength)))
            {
                Log(Log::Level::ERR) << "Failed to read compression";
                return false;
            }

            char* name = new char[decodeUInt16Little(&fileNameLength) + 1];

            if (!file.read(name, decodeUInt16Little(&fileNameLength)))
            {
                delete[] name;
                Log(Log::Level::ERR) << "Failed to read file name";
                return false;
            }

            name[decodeUInt16Little(&fileNameLength)] = '\0';

            Entry& entry = entries[name];
            entry.size = decodeUInt32Little(&uncompressedSize);

            delete[] name;

            if (!file.seek(decodeUInt16Little(&extraFieldLength), File::CURRENT))
            {
                Log(Log::Level::ERR) << "Failed to skip extra field";
                return false;
            }

            entry.offset = file.getOffset();

            if (!file.seek(decodeInt32Little(&uncompressedSize), File::CURRENT))
            {
                Log(Log::Level::ERR) << "Failed to skip extra field";
                return false;
            }
        }

        return true;
    }

    bool Archive::readFile(const std::string& filename, std::vector<uint8_t>& data) const
    {
        auto i = entries.find(filename);

        if (i == entries.end()) return false;

        if (!file.seek(static_cast<int32_t>(i->second.offset), File::BEGIN))
        {
            Log(Log::Level::ERR) << "Failed to seek file";
            return false;
        }

        data.resize(i->second.size);

        if (!file.read(data.data(), i->second.size))
        {
            Log(Log::Level::ERR) << "Failed to read file";
            return false;
        }

        return true;
    }

    bool Archive::fileExists(const std::string& filename) const
    {
        return entries.find(filename) != entries.end();
    }
}
