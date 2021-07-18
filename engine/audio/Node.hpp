// Ouzel by Elviss Strazdins

#ifndef OUZEL_AUDIO_NODE_HPP
#define OUZEL_AUDIO_NODE_HPP

#include <cstdint>
#include <algorithm>
#include <vector>

namespace ouzel::audio
{
    class Audio;

    class Node
    {
    public:
        explicit Node(Audio& initAudio):
            audio{initAudio}
        {
        }

        virtual ~Node()
        {
            if (parent)
                parent->removeChild(*this);
        }

        Node(const Node&) = delete;
        Node& operator=(const Node&) = delete;

        Node(Node&&) = delete;
        Node& operator=(Node&&) = delete;

        void addChild(Node& child);
        void removeChild(Node& child);

    private:
        Audio& audio;
        std::size_t objectId = 0;
        Node* parent = nullptr;
        std::vector<Node*> children;
    };
}

#endif // OUZEL_AUDIO_NODE_HPP
