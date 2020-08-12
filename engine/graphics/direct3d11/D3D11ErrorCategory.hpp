// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_D3D11ERRORCATEGORY_HPP
#define OUZEL_GRAPHICS_D3D11ERRORCATEGORY_HPP

#include "../../core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include <stdexcept>

namespace ouzel::graphics::d3d11
{
    class ErrorCategory final: public std::error_category
    {
    public:
        const char* name() const noexcept final
        {
            return "Direct3D11";
        }

        std::string message(int condition) const final
        {
            switch (condition)
            {
                case D3D11_ERROR_FILE_NOT_FOUND: return "D3D11_ERROR_FILE_NOT_FOUND";
                case D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS: return "D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS";
                case D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS: return "D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS";
                case D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD: return "D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD";
                case DXGI_ERROR_INVALID_CALL: return "DXGI_ERROR_INVALID_CALL";
                case DXGI_ERROR_WAS_STILL_DRAWING: return "DXGI_ERROR_WAS_STILL_DRAWING";
                case DXGI_ERROR_NOT_CURRENTLY_AVAILABLE: return "DXGI_ERROR_NOT_CURRENTLY_AVAILABLE";
                case E_FAIL: return "E_FAIL";
                case E_INVALIDARG: return "E_INVALIDARG";
                case E_OUTOFMEMORY: return "E_OUTOFMEMORY";
                case E_NOTIMPL: return "E_NOTIMPL";
                default: return "Unknown error (" + std::to_string(condition) + ")";
            }
        }
    };
}
#endif

#endif // OUZEL_GRAPHICS_D3D11ERRORCATEGORY_HPP
