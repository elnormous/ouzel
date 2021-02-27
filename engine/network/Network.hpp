// Ouzel by Elviss Strazdins

#ifndef OUZEL_NETWORK_NETWORK_HPP
#define OUZEL_NETWORK_NETWORK_HPP

#if defined(_WIN32)
#  pragma push_macro("WIN32_LEAN_AND_MEAN")
#  pragma push_macro("NOMINMAX")
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  include <winsock2.h>
#  pragma pop_macro("WIN32_LEAN_AND_MEAN")
#  pragma pop_macro("NOMINMAX")
#  include "../platform/winapi/WinSock.hpp"
#endif

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace ouzel::network
{
    class Client;

    constexpr std::uint32_t anyAddress = 0U;
    constexpr std::uint16_t anyPort = 0U;

    class Network final
    {
    public:
        Network();
        ~Network();

        Network(const Network&) = delete;
        Network& operator=(const Network&) = delete;

        Network(Network&&) = delete;
        Network& operator=(Network&&) = delete;

        static std::uint32_t getAddress(const std::string& address);

    private:
#if defined(_WIN32)
        platform::winapi::WinSock winSock;
#endif

        std::vector<std::unique_ptr<Client>> clients;
    };
}

#endif // OUZEL_NETWORK_NETWORK_HPP
