// Ouzel by Elviss Strazdins

#ifndef OUZEL_NETWORK_CLIENT_HPP
#define OUZEL_NETWORK_CLIENT_HPP

#include "Socket.hpp"
#include "../thread/Thread.hpp"

namespace ouzel::network
{
    class Network;

    class Client final
    {
    public:
        explicit Client(Network& initNetwork);
        ~Client();

        Client(const Client&) = delete;
        Client& operator=(const Client&) = delete;

        void connect(const std::string& address, std::uint16_t port);
        void disconnect();

    private:
        Network* network = nullptr;
        Socket sock;
        thread::Thread readThread;
        thread::Thread writeThread;
        bool connected = false;
    };
}

#endif // OUZEL_NETWORK_CLIENT_HPP
