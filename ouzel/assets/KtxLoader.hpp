
#ifndef OUZEL_ASSETS_KTXLOADER_HPP
#define OUZEL_ASSETS_KTXLOADER_HPP

#include "assets/Loader.hpp"

namespace ouzel
{
	namespace assets
	{
		class KtxLoader final : public Loader
		{
		public:
			explicit KtxLoader(Cache& initCache);
			bool loadAsset(Bundle& bundle,
				const std::string& name,
				const std::vector<uint8_t>& data,
				bool mipmaps = true) final;
		private:
			void getIdentifier(std::vector<uint8_t>& result);

			uint32_t getUInt32(std::vector<uint8_t>::iterator iterator, unsigned int numOfBytes);

			uint32_t getString(std::vector<uint8_t>::iterator& iterator, std::string & result);
			
			bool isGL_PALETTEFormat(uint32_t);

			std::vector<uint8_t> mData;
			std::vector<uint8_t> mIdentifier;
			uint32_t mEndianess;
			uint32_t mGlType;
			uint32_t mGlTypeSize;
			uint32_t mGlFormat;
			uint32_t mGlInternalFormat;
			uint32_t mGlBaseInternalFormat;
			uint32_t mPixelWidth;
			uint32_t mPixelHeight;
			uint32_t mPixelDepth;
			uint32_t mNumberOfArrayElements;
			uint32_t mNumberOfFaces;
			uint32_t mNumberOfMipmapLevels;
			uint32_t mBytesOfKeyValueData;
			uint32_t mKeyAndValueByteSize;
		};
	}
}
#endif // OUZEL_ASSETS_KTXLOADER_HPP

