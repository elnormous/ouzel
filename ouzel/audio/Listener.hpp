// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef LISTENER_HPP
#define LISTENER_HPP

#include <cfloat>
#include "audio/AudioDevice.hpp"
#include "audio/SoundOutput.hpp"
#include "scene/Component.hpp"
#include "math/Quaternion.hpp"
#include "math/Vector3.hpp"

namespace ouzel
{
    namespace audio
    {
        class Listener: public SoundOutput, public scene::Component
        {
            friend Audio;
        public:
            Listener();
            virtual ~Listener();

            AudioDevice::RenderCommand getRenderCommand();

        protected:
            void updateTransform() override;

            static void setAttributes(Vector3& listenerPosition,
                                      Quaternion& listenerRotation,
                                      float& pitch,
                                      float& gain,
                                      float& rolloffScale,
                                      const Vector3& position,
                                      const Quaternion& rotation);

            Audio* audio = nullptr;

            Vector3 position;
            Quaternion rotation;
            bool transformDirty = true;
        };
    } // namespace audio
} // namespace ouzel

#endif // LISTENER_HPP
