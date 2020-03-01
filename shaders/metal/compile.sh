XCODE=`xcode-select --print-path`

TOOLCHAIN="$XCODE/Toolchains/XcodeDefault.xctoolchain"

MACOS_PLATFORM="$XCODE/Platforms/MacOSX.platform"
IOS_PLATFORM="$XCODE/Platforms/iPhoneOS.platform"
TVOS_PLATFORM="$XCODE/Platforms/AppleTVOS.platform"

# macOS
MACOS_SDK=`xcrun --sdk macosx --show-sdk-path`
export SDKROOT="$MACOS_SDK"

$TOOLCHAIN/usr/bin/metal -c ColorPS.metal -target air64-apple-macos10.13 -ffast-math -o ColorPSMacOS.air -isysroot "$MACOS_SDK"
$TOOLCHAIN/usr/bin/metallib -o ColorPSMacOS.metallib ColorPSMacOS.air

$TOOLCHAIN/usr/bin/metal -c ColorVS.metal -target air64-apple-macos10.13 -ffast-math -o ColorVSMacOS.air -isysroot "$MACOS_SDK"
$TOOLCHAIN/usr/bin/metallib -o ColorVSMacOS.metallib ColorVSMacOS.air

$TOOLCHAIN/usr/bin/metal -c TexturePS.metal -target air64-apple-macos10.13 -ffast-math -o TexturePSMacOS.air -isysroot "$MACOS_SDK"
$TOOLCHAIN/usr/bin/metallib -o TexturePSMacOS.metallib TexturePSMacOS.air

$TOOLCHAIN/usr/bin/metal -c TextureVS.metal -target air64-apple-macos10.13 -ffast-math -o TextureVSMacOS.air -isysroot "$MACOS_SDK"
$TOOLCHAIN/usr/bin/metallib -o TextureVSMacOS.metallib TextureVSMacOS.air

# iOS
IOS_SDK=`xcrun --sdk iphoneos --show-sdk-path`
export SDKROOT="$IOS_SDK"

$TOOLCHAIN/usr/bin/metal -c ColorPS.metal -target air64-apple-ios11.0 -ffast-math -o ColorPSIOS.air -isysroot "$IOS_SDK"
$TOOLCHAIN/usr/bin/metallib -o ColorPSIOS.metallib ColorPSIOS.air

$TOOLCHAIN/usr/bin/metal -c ColorVS.metal -target air64-apple-ios11.0 -ffast-math -o ColorVSIOS.air -isysroot "$IOS_SDK"
$TOOLCHAIN/usr/bin/metallib -o ColorVSIOS.metallib ColorVSIOS.air

$TOOLCHAIN/usr/bin/metal -c TexturePS.metal -target air64-apple-ios11.0 -ffast-math -o TexturePSIOS.air -isysroot "$IOS_SDK"
$TOOLCHAIN/usr/bin/metallib -o TexturePSIOS.metallib TexturePSIOS.air

$TOOLCHAIN/usr/bin/metal -c TextureVS.metal -target air64-apple-ios11.0 -ffast-math -o TextureVSIOS.air -isysroot "$IOS_SDK"
$TOOLCHAIN/usr/bin/metallib -o TextureVSIOS.metallib TextureVSIOS.air

# tvOS
TVOS_SDK=`xcrun --sdk appletvos --show-sdk-path`
export SDKROOT="$TVOS_SDK"

$TOOLCHAIN/usr/bin/metal -c ColorPS.metal -target air64-apple-tvos11.3 -ffast-math -o ColorPSTVOS.air -isysroot "$TVOS_SDK"
$TOOLCHAIN/usr/bin/metallib -o ColorPSTVOS.metallib ColorPSTVOS.air

$TOOLCHAIN/usr/bin/metal -c ColorVS.metal -target air64-apple-tvos11.3 -ffast-math -o ColorVSTVOS.air -isysroot "$TVOS_SDK"
$TOOLCHAIN/usr/bin/metallib -o ColorVSTVOS.metallib ColorVSTVOS.air

$TOOLCHAIN/usr/bin/metal -c TexturePS.metal -target air64-apple-tvos11.3 -ffast-math -o TexturePSTVOS.air -isysroot "$TVOS_SDK"
$TOOLCHAIN/usr/bin/metallib -o TexturePSTVOS.metallib TexturePSTVOS.air

$TOOLCHAIN/usr/bin/metal -c TextureVS.metal -target air64-apple-tvos11.3 -ffast-math -o TextureVSTVOS.air -isysroot "$TVOS_SDK"
$TOOLCHAIN/usr/bin/metallib -o TextureVSTVOS.metallib TextureVSTVOS.air

xxd -i ColorPSMacOS.metallib ColorPSMacOS.h
xxd -i ColorVSMacOS.metallib ColorVSMacOS.h
xxd -i TexturePSMacOS.metallib TexturePSMacOS.h
xxd -i TextureVSMacOS.metallib TextureVSMacOS.h

xxd -i ColorPSIOS.metallib ColorPSIOS.h
xxd -i ColorVSIOS.metallib ColorVSIOS.h
xxd -i TexturePSIOS.metallib TexturePSIOS.h
xxd -i TextureVSIOS.metallib TextureVSIOS.h

xxd -i ColorPSTVOS.metallib ColorPSTVOS.h
xxd -i ColorVSTVOS.metallib ColorVSTVOS.h
xxd -i TexturePSTVOS.metallib TexturePSTVOS.h
xxd -i TextureVSTVOS.metallib TextureVSTVOS.h

rm -rf ./*.air
rm -rf ./*.metallib