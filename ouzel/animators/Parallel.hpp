// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef PARALLEL_HPP
#define PARALLEL_HPP

#include <vector>
#include "animators/Animator.hpp"

namespace ouzel
{
    namespace scene
    {
        class Parallel final: public Animator
        {
        public:
            explicit Parallel(const std::vector<Animator*>& initAnimators);
            explicit Parallel(const std::vector<std::unique_ptr<Animator>>& initAnimators);

        protected:
            void updateProgress() override;
        };
    } // namespace scene
} // namespace ouzel

#endif // PARALLEL_HPP
