XCODE=`xcode-select --print-path`

MACOS_PLATFORM="$XCODE/Platforms/MacOSX.platform"
IOS_PLATFORM="$XCODE/Platforms/iPhoneOS.platform"
TVOS_PLATFORM="$XCODE/Platforms/AppleTVOS.platform"

# macOS
$MACOS_PLATFORM/usr/bin/metal -arch air64 -ffast-math -o ColorPSMacOS.air -std=osx-metal1.1 ColorPS.metal
$MACOS_PLATFORM/usr/bin/metal-ar r ColorPSMacOS.metalar ColorPSMacOS.air
$MACOS_PLATFORM/usr/bin/metallib -o ColorPSMacOS.metallib ColorPSMacOS.metalar

$MACOS_PLATFORM/usr/bin/metal -arch air64 -ffast-math -o ColorVSMacOS.air -std=osx-metal1.1 ColorVS.metal
$MACOS_PLATFORM/usr/bin/metal-ar r ColorVSMacOS.metalar ColorVSMacOS.air
$MACOS_PLATFORM/usr/bin/metallib -o ColorVSMacOS.metallib ColorVSMacOS.metalar

$MACOS_PLATFORM/usr/bin/metal -arch air64 -ffast-math -o TexturePSMacOS.air -std=osx-metal1.1 TexturePS.metal
$MACOS_PLATFORM/usr/bin/metal-ar r TexturePSMacOS.metalar TexturePSMacOS.air
$MACOS_PLATFORM/usr/bin/metallib -o TexturePSMacOS.metallib TexturePSMacOS.metalar

$MACOS_PLATFORM/usr/bin/metal -arch air64 -ffast-math -o TextureVSMacOS.air -std=osx-metal1.1 TextureVS.metal
$MACOS_PLATFORM/usr/bin/metal-ar r TextureVSMacOS.metalar TextureVSMacOS.air
$MACOS_PLATFORM/usr/bin/metallib -o TextureVSMacOS.metallib TextureVSMacOS.metalar

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

xxd -i ColorPSMacOS.metallib ../../ouzel/metal/ColorPSMacOS.h
xxd -i ColorVSMacOS.metallib ../../ouzel/metal/ColorVSMacOS.h
xxd -i TexturePSMacOS.metallib ../../ouzel/metal/TexturePSMacOS.h
xxd -i TextureVSMacOS.metallib ../../ouzel/metal/TextureVSMacOS.h

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