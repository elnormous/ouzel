// Ouzel by Elviss Strazdins

#ifndef OUZEL_CORE_JNIERRORCATEGORY_HPP
#define OUZEL_CORE_JNIERRORCATEGORY_HPP

#include <system_error>
#include <jni.h>

namespace ouzel::core::android
{
    class ErrorCategory final: public std::error_category
    {
    public:
        const char* name() const noexcept override
        {
            return "JNI";
        }

        std::string message(int condition) const override
        {
            switch (condition)
            {
                case JNI_ERR: return "JNI_ERR";
                case JNI_EDETACHED: return "JNI_EDETACHED";
                case JNI_EVERSION: return "JNI_EVERSION";
                case JNI_ENOMEM: return "JNI_ENOMEM";
                case JNI_EEXIST: return "JNI_EEXIST";
                case JNI_EINVAL: return "JNI_EINVAL";
                default: return "Unknown error (" + std::to_string(condition) + ")";
            }
        }
    };
}

#endif // OUZEL_CORE_JNIERRORCATEGORY_HPP
