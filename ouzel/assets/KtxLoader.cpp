#if defined(_MSC_VER)
#  pragma warning( push )
#  pragma warning( disable : 4100 )
#  pragma warning( disable : 4505 )
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wdouble-promotion"
#  pragma GCC diagnostic ignored "-Wmissing-prototypes"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsign-conversion"
#  pragma GCC diagnostic ignored "-Wunused-function"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  if defined(__clang__)
#    pragma GCC diagnostic ignored "-Wcomma"
#  endif
#endif

#if TARGET_OS_MAC
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#else
	#ifdef _WIN32
		#include <windows.h>
	#endif
	#include <gl/glext.h>
#endif

#include "KtxLoader.hpp"
#include <algorithm>
#include "Bundle.hpp"
#include "core/Engine.hpp"
#include "graphics/Image.hpp"
#include "graphics/Texture.hpp"

namespace ouzel
{
	namespace assets
	{
		KtxLoader::KtxLoader(Cache & initCache) 
			: Loader(initCache, Loader::Ktx)
		{
		}

		bool KtxLoader::loadAsset(Bundle & bundle, const std::string & name, const std::vector<uint8_t>& data, bool mipmaps)
		{
			mData = std::move(data);
			std::vector<uint8_t> imageData;

			auto iterator = mData.begin();
			auto offset = mData.begin();

			while (iterator != mData.end())
			{
				getIdentifier(mIdentifier);
				iterator += 12;
				
				mEndianess = getUInt32(iterator, 4);
				iterator += 4;

				mGlType = getUInt32(iterator, 4);
				iterator += 4;

				mGlTypeSize = getUInt32(iterator, 4);
				iterator += 4;

				mGlFormat = getUInt32(iterator, 4);
				iterator += 4;

				mGlInternalFormat = getUInt32(iterator, 4);
				iterator += 4;

				mGlBaseInternalFormat = getUInt32(iterator, 4);
				iterator += 4;

				mPixelWidth = getUInt32(iterator, 4);
				iterator += 4;

				mPixelHeight = getUInt32(iterator, 4);
				iterator += 4;

				mPixelDepth = getUInt32(iterator, 4);
				iterator += 4;

				mNumberOfArrayElements = getUInt32(iterator, 4);
				iterator += 4;

				mNumberOfFaces = getUInt32(iterator, 4);
				iterator += 4;

				mNumberOfMipmapLevels = getUInt32(iterator, 4);
				iterator += 4;

				mBytesOfKeyValueData = getUInt32(iterator, 4);
				iterator += 4;

				mKeyAndValueByteSize = getUInt32(iterator,4);
				iterator += 4;

				std::string key = "";
				int length = 0;
				length = getString(iterator, key);
	
				std::string value = "";
				length += getString(iterator, value);

				//iterator += bytesOfKeyValueData - length;
				iterator += 3 - ((mKeyAndValueByteSize + 3) % 4);

				bool isPalette = false;
				if (mNumberOfMipmapLevels == 0 || (isPalette = isGL_PALETTEFormat(mGlInternalFormat)))
				{
					mNumberOfMipmapLevels = 1;
				}

				if (isPalette && !mNumberOfFaces)
				{
					mNumberOfFaces = 1;
				}

				for (uint32_t i = 0; i < mNumberOfMipmapLevels; ++i)
				{
					uint32_t imageSize = getUInt32(iterator, 4);
					iterator += 4;
					imageData.resize(imageSize);

					for (uint32_t j = 0; j < std::max<uint32_t>(1, mNumberOfArrayElements); ++j)
					{
						for (uint32_t k = 0; k < mNumberOfFaces; ++k)
						{
							for (uint32_t z = 0; z < std::max<uint32_t>(1, mPixelDepth); ++z)
							{
								for (uint32_t h = 0; h < std::max<uint32_t>(1, mPixelHeight); ++h)
								{
									for (uint32_t w = 0; w < mPixelWidth; ++w)
									{
										int index = (i + mNumberOfMipmapLevels * (j + std::max<uint32_t>(1, mNumberOfArrayElements) * (k + mNumberOfFaces * (z + std::max<uint32_t>(1, mPixelDepth) * ((h * mPixelWidth + w)*4)))));
										offset = iterator + index;
										imageData[index] = *offset;
										imageData[index+1] = *(offset+1);
										imageData[index+2] = *(offset+2);
										imageData[index+3] = *(offset+3);
									}
								}
							}
						}
					}
					iterator += imageSize;
				}
			}

			graphics::Image image(graphics::PixelFormat::RGBA8UNorm,
				Size2U(static_cast<uint32_t>(mPixelWidth),
					static_cast<uint32_t>(mPixelHeight)),
				imageData);

			auto texture = std::make_shared<graphics::Texture>(*engine->getRenderer(),
				image.getData(),
				image.getSize(), 0,
				mNumberOfMipmapLevels ? 0 : 1,
				image.getPixelFormat());

			bundle.setTexture(name, texture);

			return true;
		}

		void KtxLoader::getIdentifier(std::vector<uint8_t>& result) 
		{
			result.resize(12);
			std::memcpy(&result[0],&mData[0],12);
		}

		uint32_t KtxLoader::getUInt32(std::vector<uint8_t>::iterator iterator, unsigned int numOfBytes)
		{
			uint32_t value = 0;

			std::memcpy(&value,&(*iterator),numOfBytes);
			return value;
		}

		uint32_t KtxLoader::getString(std::vector<uint8_t>::iterator& iterator, std::string& result)
		{
			int numberOfReadBytes = 0;
			while (*iterator != '\0')
			{
				result.push_back(*iterator);
				++iterator;
				++numberOfReadBytes;
			}
			++iterator; //We need to increment to take into accout the null character
			return numberOfReadBytes + 1;
		}

		bool KtxLoader::isGL_PALETTEFormat(uint32_t value)
		{
			switch (value)
			{
			case GL_PALETTE4_RGB8_OES: return true;
			case GL_PALETTE4_RGBA8_OES: return true;
			case GL_PALETTE4_R5_G6_B5_OES: return true;
			case GL_PALETTE4_RGBA4_OES: return true;
			case GL_PALETTE4_RGB5_A1_OES: return true;
			case GL_PALETTE8_RGB8_OES: return true;
			case GL_PALETTE8_RGBA8_OES: return true;
			case GL_PALETTE8_R5_G6_B5_OES:return true;
			case GL_PALETTE8_RGBA4_OES: return true;
			case GL_PALETTE8_RGB5_A1_OES: return true;
			default:
				break;
			}
			return false;
		}
	}
}