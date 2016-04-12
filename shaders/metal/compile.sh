xcrun metal -arch air64 -ffast-math -o ColorPS.air -std=osx-metal1.1 ColorPS.metal
xcrun metal-ar r ColorPS.metalar ColorPS.air
xcrun metallib -o ColorPS.metallib ColorPS.metalar

xcrun metal -arch air64 -ffast-math -o ColorVS.air -std=osx-metal1.1 ColorVS.metal
xcrun metal-ar r ColorVS.metalar ColorVS.air
xcrun metallib -o ColorVS.metallib ColorVS.metalar

xcrun metal -arch air64 -ffast-math -o TexturePS.air -std=osx-metal1.1 TexturePS.metal
xcrun metal-ar r TexturePS.metalar TexturePS.air
xcrun metallib -o TexturePS.metallib TexturePS.metalar

xcrun metal -arch air64 -ffast-math -o TextureVS.air -std=osx-metal1.1 TextureVS.metal
xcrun metal-ar r TextureVS.metalar TextureVS.air
xcrun metallib -o TextureVS.metallib TextureVS.metalar