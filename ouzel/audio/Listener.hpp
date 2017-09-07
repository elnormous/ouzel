// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "audio/SoundOutput.hpp"
#include "math/Quaternion.hpp"
#include "math/Vector3.hpp"

namespace ouzel
{
    namespace audio
    {
        class Audio;

        class Listener: public SoundOutput
        {
            friend Audio;
        public:
            Listener();
            virtual ~Listener();

            const Vector3& getPosition() const { return position; }
            void setPosition(const Vector3& newPosition) { position = newPosition; }

            const Quaternion& getRotation() const { return rotation; }
            void setRotation(const Quaternion& newRotation) { rotation = newRotation; }

        protected:
            Vector3 position;
            Quaternion rotation;

            Audio* audio = nullptr;
        };
    } // namespace audio
} // namespace ouzel
