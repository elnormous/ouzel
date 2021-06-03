// Ouzel by Elviss Strazdins

#ifndef OUZEL_STORAGE_PATH_HPP
#define OUZEL_STORAGE_PATH_HPP

#include <chrono>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace ouzel::storage
{
    class Path final
    {
    public:
        enum class Format
        {
            generic,
            native
        };

#ifdef _WIN32
        using Char = wchar_t;
        using String = std::wstring;
        static constexpr Char preferredSeparator = '\\';
        static constexpr const Char* parentDirectory = L"..";
        static constexpr const Char* currentDirectory = L".";
#elif defined(__unix__) || defined(__APPLE__)
        using Char = char;
        using String = std::string;
        static constexpr Char preferredSeparator = '/';
        static constexpr const Char* parentDirectory = "..";
        static constexpr const Char* currentDirectory = ".";
#endif

        Path() = default;

        Path(const Path& p):
            path{p.path}
        {
        }

        Path(Path&& p) noexcept:
            path{std::move(p.path)}
        {
        }

        Path(String&& p) noexcept:
            path{std::move(p)}
        {
        }

        template <class Source>
        Path(const Source& p, Format format = Format::generic):
            path{format == Format::generic ? convertToNative(p) : encode(p)}
        {
        }

        Path& operator=(const Path& other)
        {
            if (&other == this) return *this;
            path = other.path;
            return *this;
        }

        Path& operator=(Path&& other) noexcept
        {
            if (&other == this) return *this;
            path = std::move(other.path);
            return *this;
        }

        operator std::string() const
        {
            return convertToUtf8(path);
        }

        bool operator==(const Path& p) const noexcept
        {
            return path == p.path;
        }

        template <class Source>
        bool operator==(const Source& p) const
        {
            return path == convertToNative(p);
        }

        bool operator!=(const Path& p) const noexcept
        {
            return path != p.path;
        }

        Path& operator+=(const Path& p)
        {
            path += p.path;
            return *this;
        }

        template <class Source>
        Path& operator+=(const Source& p)
        {
            path += convertToNative(p);
            return *this;
        }

        Path& operator/=(const Path& p)
        {
            if (!path.empty() && !isDirectorySeparator(path.back()))
                path += preferredSeparator;
            path += p.path;
            return *this;
        }

        template <class Source>
        Path& operator/=(const Source& p)
        {
            if (!path.empty() && !isDirectorySeparator(path.back()))
                path += preferredSeparator;
            path += convertToNative(p);
            return *this;
        }

        Path operator+(const Path& p) const
        {
            Path result = *this;
            result.path += p.path;
            return result;
        }

        template <class Source>
        Path operator+(const Source& p) const
        {
            Path result = *this;
            result.path += convertToNative(p);
            return result;
        }

        Path operator/(const Path& p) const
        {
            Path result = *this;
            if (!result.path.empty() && !isDirectorySeparator(result.path.back()))
                result.path += preferredSeparator;
            result.path += p.path;
            return result;
        }

        template <class Source>
        Path operator/(const Source& p) const
        {
            Path result = *this;
            if (!result.path.empty() && !isDirectorySeparator(result.path.back()))
                result.path += preferredSeparator;
            result.path += convertToNative(p);
            return result;
        }

        bool isEmpty() const noexcept
        {
            return path.empty();
        }

        const String& getNative() const noexcept
        {
            return path;
        }

        std::string getGeneric() const
        {
            return convertToGeneric(path);
        }

        Path getExtension() const
        {
            if (const std::size_t position = path.rfind(Char('.')); position != std::string::npos)
                return Path{path.substr(position + 1)};
            else
                return Path{};
        }

        template <class Source>
        Path& replaceExtension(const Source& extension)
        {
            if (const std::size_t position = path.rfind(Char('.')); position != std::string::npos)
                path.resize(position + 1);
            path += convertToNative(extension);
            return *this;
        }

        Path& removeExtension()
        {
            if (const std::size_t position = path.rfind(Char('.')); position != std::string::npos)
                path.resize(position);
            return *this;
        }

        Path getFilename() const
        {
            if (const std::size_t position = findLastDirectorySeparator(path); position != String::npos)
                return Path{path.substr(position + 1)};
            else
                return Path{path};
        }

        template <class Source>
        Path& replaceFilename(const Source& filename)
        {
            if (const std::size_t position = findLastDirectorySeparator(path); position != std::string::npos)
                path.resize(position + 1);
            else
                path.clear();
            path += convertToNative(filename);
            return *this;
        }

        Path& removeFilename()
        {
            if (const std::size_t position = findLastDirectorySeparator(path); position != std::string::npos)
                path.resize(position + 1);
            else
                path.clear();
            return *this;
        }

        Path getStem() const
        {
            const std::size_t directoryPosition = findLastDirectorySeparator(path);
            const std::size_t startPosition = directoryPosition == String::npos ? 0 : directoryPosition + 1;
            const std::size_t extensionPosition = path.find(Char('.'), startPosition);
            const std::size_t endPosition = extensionPosition == String::npos ? path.size() : extensionPosition;

            Path result;
            result.path = path.substr(startPosition, endPosition - startPosition);
            return result;
        }

        Path getDirectory() const
        {
            if (const std::size_t position = findLastDirectorySeparator(path); position != String::npos)
                return Path{path.substr(0, position)};
            else
                return Path{};
        }

        Path getRoot() const
        {
            Path result;
#ifdef _WIN32
            if (path.size() >= 2 &&
                ((path[0] >= L'a' && path[0] <= L'z') || (path[0] >= L'A' && path[0] <= L'Z')) &&
                path[1] == L':')
                result.path = {path[0], L':'};
#elif defined(__unix__) || defined(__APPLE__)
            if (path.size() >= 1 && path[0] == '/') result.path = '/';
#endif
            return result;
        }

        void normalize()
        {
            std::vector<String> parts;
            String newPath;
            String::size_type previousPosition = 0;

#ifdef _WIN32
            if (path.size() >= 2 &&
                ((path[0] >= L'a' && path[0] <= L'z') || (path[0] >= L'A' && path[0] <= L'Z')) &&
                path[1] == L':')
            {
                parts.push_back({path[0], L':'});
                previousPosition = 2;

                if (path.size() >= 3)
                    parts.push_back(L"\\");
            }
#elif defined(__unix__) || defined(__APPLE__)
            if (path.size() >= 1 && path[0] == '/')
            {
                parts.push_back("/");
                previousPosition = 1;
            }
#endif

            while (previousPosition < path.length())
            {
                if (isDirectorySeparator(path[previousPosition])) ++previousPosition;

                const auto position = findFirstDirectorySeparator(path, previousPosition);
                const auto endPosition = position != String::npos ? position : path.length();
                const auto currentPart = path.substr(previousPosition, endPosition - previousPosition);

                if (currentPart == parentDirectory)
                {
                    if (parts.empty())
                        parts.push_back(currentPart);
                    else
                    {
                        const auto& previousPart = parts.back();
                        if (previousPart != parentDirectory &&
                            previousPart.length() != 1 && !isDirectorySeparator(previousPart.back()))
                            parts.pop_back();
                    }
                }
                else if (currentPart.empty())
                {
                    if (position == String::npos) // the last part
                        parts.push_back(currentPart);
                }
                else if (currentPart != currentDirectory)
                    parts.push_back(currentPart);

                previousPosition = endPosition;
            }

            for (const auto& part : parts)
            {
                if (!newPath.empty() && !isDirectorySeparator(newPath.back()) &&
                    (part.length() != 1 || !isDirectorySeparator(part.back())))
                    newPath += preferredSeparator;

                newPath += part;
            }

            path = newPath;
        }

        Path getNormal() const
        {
            Path result = *this;
            result.normalize();
            return result;
        }

        bool isAbsolute() const noexcept
        {
#ifdef _WIN32
            return path.size() >= 2 &&
                ((path[0] >= L'a' && path[0] <= L'z') || (path[0] >= L'A' && path[0] <= L'Z')) &&
                path[1] == L':';
#elif defined(__unix__) || defined(__APPLE__)
            return path.size() >= 1 && path[0] == '/';
#endif
        }

        bool isRelative() const noexcept
        {
            return !isAbsolute();
        }

    private:
        static constexpr bool isDirectorySeparator(Char c) noexcept
        {
#ifdef _WIN32
            return c == L'\\' || c == L'/';
#elif defined(__unix__) || defined(__APPLE__)
            return c == '/';
#endif
        }

        static std::size_t findLastDirectorySeparator(const String& str,
                                                      std::size_t position = String::npos) noexcept
        {
#ifdef _WIN32
            return str.find_last_of(L"\\/", position);
#elif defined(__unix__) || defined(__APPLE__)
            return str.rfind('/', position);
#endif
        }

        static std::size_t findFirstDirectorySeparator(const String& str,
                                                       std::size_t position = 0) noexcept
        {
#ifdef _WIN32
            return str.find_first_of(L"\\/", position);
#elif defined(__unix__) || defined(__APPLE__)
            return str.find('/', position);
#endif
        }

        static const std::string& convertToUtf8(const std::string& p) noexcept
        {
            return p;
        }

        static std::string convertToUtf8(wchar_t c)
        {
            std::string s;

            if (c <= 0x7F)
                s.push_back(static_cast<char>(c));
            else if (c <= 0x7FF)
            {
                s.push_back(static_cast<char>(0xC0 | ((c >> 6) & 0x1F)));
                s.push_back(static_cast<char>(0x80 | (c & 0x3F)));
            }
            else if (c <= 0xFFFF)
            {
                s.push_back(static_cast<char>(0xE0 | ((c >> 12) & 0x0F)));
                s.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3F)));
                s.push_back(static_cast<char>(0x80 | (c & 0x3F)));
            }
#if WCHAR_MAX > 0xFFFF
            else
            {
                s.push_back(static_cast<char>(0xF0 | ((c >> 18) & 0x07)));
                s.push_back(static_cast<char>(0x80 | ((c >> 12) & 0x3F)));
                s.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3F)));
                s.push_back(static_cast<char>(0x80 | (c & 0x3F)));
            }
#endif

            return s;
        }

        static std::string convertToUtf8(const std::wstring& p)
        {
            std::string s;

            for (wchar_t c : p)
            {
                if (c <= 0x7F)
                    s.push_back(static_cast<char>(c));
                else if (c <= 0x7FF)
                {
                    s.push_back(static_cast<char>(0xC0 | ((c >> 6) & 0x1F)));
                    s.push_back(static_cast<char>(0x80 | (c & 0x3F)));
                }
                else if (c <= 0xFFFF)
                {
                    s.push_back(static_cast<char>(0xE0 | ((c >> 12) & 0x0F)));
                    s.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3F)));
                    s.push_back(static_cast<char>(0x80 | (c & 0x3F)));
                }
#if WCHAR_MAX > 0xFFFF
                else
                {
                    s.push_back(static_cast<char>(0xF0 | ((c >> 18) & 0x07)));
                    s.push_back(static_cast<char>(0x80 | ((c >> 12) & 0x3F)));
                    s.push_back(static_cast<char>(0x80 | ((c >> 6) & 0x3F)));
                    s.push_back(static_cast<char>(0x80 | (c & 0x3F)));
                }
#endif
            }

            return s;
        }

#ifdef _WIN32
        static std::wstring convertToWchar(const std::string& p)
        {
            std::wstring s;

            for (auto i = p.begin(); i != p.end(); ++i)
            {
                char32_t cp = *i & 0xFF;

                if (cp <= 0x7F) // length = 1
                {
                    // do nothing
                }
                else if ((cp >> 5) == 0x6) // length = 2
                {
                    if (++i == p.end())
                        throw std::runtime_error("Invalid UTF-8 string");
                    cp = ((cp << 6) & 0x7FF) + (*i & 0x3F);
                }
                else if ((cp >> 4) == 0xE) // length = 3
                {
                    if (++i == p.end())
                        throw std::runtime_error("Invalid UTF-8 string");
                    cp = ((cp << 12) & 0xFFFF) + (((*i & 0xFF) << 6) & 0x0FFF);
                    if (++i == p.end())
                        throw std::runtime_error("Invalid UTF-8 string");
                    cp += *i & 0x3F;
                }
                else if ((cp >> 3) == 0x1E) // length = 4
                {
                    if (++i == p.end())
                        throw std::runtime_error("Invalid UTF-8 string");
                    cp = ((cp << 18) & 0x1FFFFF) + (((*i & 0xFF) << 12) & 0x3FFFF);
                    if (++i == p.end())
                        throw std::runtime_error("Invalid UTF-8 string");
                    cp += ((*i & 0xFF) << 6) & 0x0FFF;
                    if (++i == p.end())
                        throw std::runtime_error("Invalid UTF-8 string");
                    cp += (*i) & 0x3F;
                }

                if (cp > WCHAR_MAX)
                    throw std::runtime_error("Unsupported UTF-8 character");

                s.push_back(static_cast<wchar_t>(cp));
            }

            return s;
        }

        static std::wstring convertToNative(char c)
        {
            const wchar_t result = (c == '/') ? L'\\' : static_cast<wchar_t>(c);
            return std::wstring{result};
        }

        static std::wstring convertToNative(const std::string& p)
        {
            std::wstring result = convertToWchar(p);

            for (auto& c : result) if (c == L'/') c = L'\\';
            return result;
        }

        static std::wstring convertToNative(wchar_t c)
        {
            const wchar_t result = (c == L'/') ? L'\\' : c;
            return std::wstring{result};
        }

        static std::wstring convertToNative(const std::wstring& p)
        {
            std::wstring result = p;

            for (auto& c : result) if (c == L'/') c = L'\\';
            return result;
        }

        static std::string convertToGeneric(const std::wstring& p)
        {
            std::string result = convertToUtf8(p);

            for (auto& c : result) if (c == '\\') c = '/';
            return result;
        }

        static std::wstring encode(char c)
        {
            return std::wstring{static_cast<wchar_t>(c)};
        }

        static std::wstring encode(const std::string& p)
        {
            return convertToWchar(p);
        }

        static std::wstring encode(wchar_t c)
        {
            return std::wstring{c};
        }

        static const std::wstring& encode(const std::wstring& p) noexcept
        {
            return p;
        }
#elif defined(__unix__) || defined(__APPLE__)
        static std::string convertToNative(char c)
        {
            return std::string{c};
        }

        static const std::string& convertToNative(const std::string& p) noexcept
        {
            return p;
        }

        static std::string convertToNative(wchar_t c)
        {
            return convertToUtf8(c);
        }

        static std::string convertToNative(const std::wstring& p)
        {
            return convertToUtf8(p);
        }

        static const std::string& convertToGeneric(const std::string& p) noexcept
        {
            return p;
        }

        static std::string encode(char c)
        {
            return std::string{c};
        }

        static const std::string& encode(const std::string& p) noexcept
        {
            return p;
        }

        static std::string encode(wchar_t c)
        {
            return convertToUtf8(c);
        }

        static std::string encode(const std::wstring& p)
        {
            return convertToUtf8(p);
        }
#endif

        String path;
    };
}

#endif
