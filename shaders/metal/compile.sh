XCODE=`xcode-select --print-path`

OSX_PLATFORM="$XCODE/Platforms/MacOSX.platform"
IOS_PLATFORM="$XCODE/Platforms/iPhoneOS.platform"
TVOS_PLATFORM="$XCODE/Platforms/AppleTVOS.platform"

# OSX
$OSX_PLATFORM/usr/bin/metal -arch air64 -ffast-math -o ColorPSOSX.air -std=osx-metal1.1 ColorPS.metal
$OSX_PLATFORM/usr/bin/metal-ar r ColorPSOSX.metalar ColorPSOSX.air
$OSX_PLATFORM/usr/bin/metallib -o ColorPSOSX.metallib ColorPSOSX.metalar

$OSX_PLATFORM/usr/bin/metal -arch air64 -ffast-math -o ColorVSOSX.air -std=osx-metal1.1 ColorVS.metal
$OSX_PLATFORM/usr/bin/metal-ar r ColorVSOSX.metalar ColorVSOSX.air
$OSX_PLATFORM/usr/bin/metallib -o ColorVSOSX.metallib ColorVSOSX.metalar

$OSX_PLATFORM/usr/bin/metal -arch air64 -ffast-math -o TexturePSOSX.air -std=osx-metal1.1 TexturePS.metal
$OSX_PLATFORM/usr/bin/metal-ar r TexturePSOSX.metalar TexturePSOSX.air
$OSX_PLATFORM/usr/bin/metallib -o TexturePSOSX.metallib TexturePSOSX.metalar

$OSX_PLATFORM/usr/bin/metal -arch air64 -ffast-math -o TextureVSOSX.air -std=osx-metal1.1 TextureVS.metal
$OSX_PLATFORM/usr/bin/metal-ar r TextureVSOSX.metalar TextureVSOSX.air
$OSX_PLATFORM/usr/bin/metallib -o TextureVSOSX.metallib TextureVSOSX.metalar

# iOS

$IOS_PLATFORM/usr/bin/metal -arch air64 -ffast-math -o ColorPSIOS.air -std=ios-metal1.1 ColorPS.metal
$IOS_PLATFORM/usr/bin/metal-ar r ColorPSIOS.metalar ColorPSIOS.air
$IOS_PLATFORM/usr/bin/metallib -o ColorPSIOS.metallib ColorPSIOS.metalar

$IOS_PLATFORM/usr/bin/metal -arch air64 -ffast-math -o ColorVSIOS.air -std=ios-metal1.1 ColorVS.metal
$IOS_PLATFORM/usr/bin/metal-ar r ColorVSIOS.metalar ColorVSIOS.air
$IOS_PLATFORM/usr/bin/metallib -o ColorVSIOS.metallib ColorVSIOS.metalar

$IOS_PLATFORM/usr/bin/metal -arch air64 -ffast-math -o TexturePSIOS.air -std=ios-metal1.1 TexturePS.metal
$IOS_PLATFORM/usr/bin/metal-ar r TexturePSIOS.metalar TexturePSIOS.air
$IOS_PLATFORM/usr/bin/metallib -o TexturePSIOS.metallib TexturePSIOS.metalar

$IOS_PLATFORM/usr/bin/metal -arch air64 -ffast-math -o TextureVSIOS.air -std=ios-metal1.1 TextureVS.metal
$IOS_PLATFORM/usr/bin/metal-ar r TextureVSIOS.metalar TextureVSIOS.air
$IOS_PLATFORM/usr/bin/metallib -o TextureVSIOS.metallib TextureVSIOS.metalar

# tvOS

$TVOS_PLATFORM/usr/bin/metal -arch air64 -ffast-math -o ColorPSTVOS.air -std=ios-metal1.1 ColorPS.metal
$TVOS_PLATFORM/usr/bin/metal-ar r ColorPSTVOS.metalar ColorPSTVOS.air
$TVOS_PLATFORM/usr/bin/metallib -o ColorPSTVOS.metallib ColorPSTVOS.metalar

$TVOS_PLATFORM/usr/bin/metal -arch air64 -ffast-math -o ColorVSTVOS.air -std=ios-metal1.1 ColorVS.metal
$TVOS_PLATFORM/usr/bin/metal-ar r ColorVSTVOS.metalar ColorVSTVOS.air
$TVOS_PLATFORM/usr/bin/metallib -o ColorVSTVOS.metallib ColorVSTVOS.metalar

$TVOS_PLATFORM/usr/bin/metal -arch air64 -ffast-math -o TexturePSTVOS.air -std=ios-metal1.1 TexturePS.metal
$TVOS_PLATFORM/usr/bin/metal-ar r TexturePSTVOS.metalar TexturePSTVOS.air
$TVOS_PLATFORM/usr/bin/metallib -o TexturePSTVOS.metallib TexturePSTVOS.metalar

$TVOS_PLATFORM/usr/bin/metal -arch air64 -ffast-math -o TextureVSTVOS.air -std=ios-metal1.1 TextureVS.metal
$TVOS_PLATFORM/usr/bin/metal-ar r TextureVSTVOS.metalar TextureVSTVOS.air
$TVOS_PLATFORM/usr/bin/metallib -o TextureVSTVOS.metallib TextureVSTVOS.metalar

xxd -i ColorPSOSX.metallib ../../ouzel/metal/ColorPSOSX.h
xxd -i ColorVSOSX.metallib ../../ouzel/metal/ColorVSOSX.h
xxd -i TexturePSOSX.metallib ../../ouzel/metal/TexturePSOSX.h
xxd -i TextureVSOSX.metallib ../../ouzel/metal/TextureVSOSX.h

xxd -i ColorPSIOS.metallib ../../ouzel/metal/ColorPSIOS.h
xxd -i ColorVSIOS.metallib ../../ouzel/metal/ColorVSIOS.h
xxd -i TexturePSIOS.metallib ../../ouzel/metal/TexturePSIOS.h
xxd -i TextureVSIOS.metallib ../../ouzel/metal/TextureVSIOS.h

xxd -i ColorPSTVOS.metallib ../../ouzel/metal/ColorPSTVOS.h
xxd -i ColorVSTVOS.metallib ../../ouzel/metal/ColorVSTVOS.h
xxd -i TexturePSTVOS.metallib ../../ouzel/metal/TexturePSTVOS.h
xxd -i TextureVSTVOS.metallib ../../ouzel/metal/TextureVSTVOS.h

rm -rf ./*.air
rm -rf ./*.metalar
rm -rf ./*.metallib