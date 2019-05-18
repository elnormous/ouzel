// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_MIXER_OBJECT_HPP
#define OUZEL_AUDIO_MIXER_OBJECT_HPP

#include <cstdint>
#include <vector>
#include "math/Quaternion.hpp"
#include "math/Vector.hpp"

namespace ouzel
{
    namespace audio
    {
        namespace mixer
        {
            class Object
            {
            public:
                Object() = default;
                virtual ~Object()
                {
                    if (parent)
                        parent->removeChild(*this);
                }

                Object(const Object&) = delete;
                Object& operator=(const Object&) = delete;

                Object(Object&&) = delete;
                Object& operator=(Object&&) = delete;

                void addChild(Object& child)
                {
                    if (child.parent != this)
                    {
                        if (child.parent)
                            child.parent->removeChild(child);

                        auto i = std::find(children.begin(), children.end(), &child);
                        if (i == children.end())
                        {
                            child.parent = this;
                            children.push_back(&child);
                        }
                    }
                }

                void removeChild(Object& child)
                {
                    if (child.parent == this)
                    {
                        auto i = std::find(children.begin(), children.end(), &child);
                        if (i != children.end())
                        {
                            child.parent = this;
                            children.erase(i);
                        }
                    }
                }

            private:
                Object* parent = nullptr;
                std::vector<Object*> children;
            };
        }
    } // namespace audio
} // namespace ouzel

#endif // OUZEL_AUDIO_MIXER_OBJECT_HPP
