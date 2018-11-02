// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Cursor.hpp"
#include "InputManager.hpp"
#include "core/Engine.hpp"
#include "utils/Errors.hpp"
#include "stb_image.h"

namespace ouzel
{
    namespace input
    {
        Cursor::Cursor(InputManager& initInputManager):
            inputManager(initInputManager)
        {
            cursorResource = inputManager.getInputSystem()->getResourceId();
        }

        Cursor::Cursor(InputManager& initInputManager, SystemCursor systemCursor):
            Cursor(initInputManager)
        {
            init(systemCursor);
        }

        Cursor::Cursor(InputManager& initInputManager, const std::string& filename, const Vector2& hotSpot):
            Cursor(initInputManager)
        {
            init(filename, hotSpot);
        }

        Cursor::~Cursor()
        {
            if (cursorResource)
            {
                InputSystem::Command command;
                command.type = InputSystem::Command::Type::DESTROY_CURSOR;
                command.cursorResource = cursorResource;
                inputManager.getInputSystem()->addCommand(command);

                inputManager.getInputSystem()->deleteResourceId(cursorResource);
            }
        }

        void Cursor::init(SystemCursor systemCursor)
        {
            InputSystem::Command command;
            command.type = InputSystem::Command::Type::INIT_CURSOR;
            command.cursorResource = cursorResource;
            command.systemCursor = systemCursor;
            inputManager.getInputSystem()->addCommand(command);
        }

        void Cursor::init(const std::string& filename, const Vector2& hotSpot)
        {
            // TODO: load with asset loader
            std::vector<uint8_t> data = engine->getFileSystem().readFile(filename);

            int width;
            int height;
            int comp;

            stbi_uc* tempData = stbi_load_from_memory(data.data(), static_cast<int>(data.size()), &width, &height, &comp, STBI_default);

            if (!tempData)
                throw ParseError("Failed to load texture, reason: " + std::string(stbi_failure_reason()));

            size_t pixelSize;
            graphics::PixelFormat pixelFormat;
            std::vector<uint8_t> imageData;

            switch (comp)
            {
                case STBI_grey:
                {
                    pixelFormat = graphics::PixelFormat::R8_UNORM;
                    pixelSize = 1;
                    imageData.assign(tempData,
                                     tempData + static_cast<size_t>(width * height) * pixelSize);
                    stbi_image_free(tempData);
                    break;
                }
                case STBI_grey_alpha:
                {
                    pixelFormat = graphics::PixelFormat::RG8_UNORM;
                    pixelSize = 2;
                    imageData.assign(tempData,
                                     tempData + static_cast<size_t>(width * height) * pixelSize);
                    stbi_image_free(tempData);
                    break;
                }
                case STBI_rgb:
                {
                    pixelFormat = graphics::PixelFormat::RGBA8_UNORM;
                    pixelSize = 4;

                    imageData.resize(static_cast<size_t>(width * height * 4));

                    for (int y = 0; y < height; ++y)
                    {
                        for (int x = 0; x < width; ++x)
                        {
                            size_t sourceOffset = static_cast<size_t>((y * height + x) * 3);
                            size_t destinationOffset = static_cast<size_t>((y * height + x) * 4);
                            imageData[destinationOffset + 0] = tempData[sourceOffset + 0];
                            imageData[destinationOffset + 1] = tempData[sourceOffset + 1];
                            imageData[destinationOffset + 2] = tempData[sourceOffset + 2];
                            imageData[destinationOffset + 3] = 255;
                        }
                    }
                    stbi_image_free(tempData);
                    break;
                }
                case STBI_rgb_alpha:
                {
                    pixelFormat = graphics::PixelFormat::RGBA8_UNORM;
                    pixelSize = 4;
                    imageData.assign(tempData,
                                     tempData + static_cast<size_t>(width * height) * pixelSize);
                    stbi_image_free(tempData);
                    break;
                }
                default:
                    stbi_image_free(tempData);
                    throw ParseError("Unsupported pixel size");
            }

            init(imageData,
                 Size2(static_cast<float>(width), static_cast<float>(height)),
                 pixelFormat,
                 hotSpot);
        }

        void Cursor::init(const std::vector<uint8_t>& data,
                          const Size2& size,
                          graphics::PixelFormat pixelFormat,
                          const Vector2& hotSpot)
        {
            InputSystem::Command command;
            command.cursorResource = cursorResource;
            command.type = InputSystem::Command::Type::INIT_CURSOR;
            command.data = data;
            command.size = size;
            command.pixelFormat = pixelFormat;
            command.hotSpot = hotSpot;
            inputManager.getInputSystem()->addCommand(command);
        }
    } // namespace input
} // namespace ouzel
