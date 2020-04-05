// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_PBXFILETYPE_HPP
#define OUZEL_XCODE_PBXFILETYPE_HPP

#include <string>

namespace ouzel
{
    namespace xcode
    {
        enum class PbxFileType
        {
            ArchiveAr,
            ArchiveAsdictionary,
            ArchiveBinhex,
            ArchiveEar,
            ArchiveGzip,
            ArchiveJar,
            ArchiveMacbinary,
            ArchivePpob,
            ArchiveRsrc,
            ArchiveStuffit,
            ArchiveTar,
            ArchiveWar,
            ArchiveZip,
            AudioAiff,
            AudioAu,
            AudioMidi,
            AudioMp3,
            AudioWav,
            CompiledMachO,
            CompiledMachOBundle,
            CompiledMachOCorefile,
            CompiledMachODylib,
            CompiledMachOExecutable,
            CompiledMachOFvmlib,
            CompiledMachOObjfile,
            CompiledMachOPreload,
            FileBplist,
            FileXib,
            ImageBmp,
            ImageGif,
            ImageIcns,
            ImageIco,
            ImageJpeg,
            ImagePdf,
            ImagePict,
            ImagePng,
            ImageTiff,
            PatternProxy,
            SourcecodeAda,
            SourcecodeApplescript,
            SourcecodeAsm,
            SourcecodeAsmAsm,
            SourcecodeAsmLlvm,
            SourcecodeC,
            SourcecodeCCPreprocessed,
            SourcecodeCH,
            SourcecodeCObjc,
            SourcecodeCObjcPreprocessed,
            SourcecodeCppCpp,
            SourcecodeCppCppPreprocessed,
            SourcecodeCppH,
            SourcecodeCppObjcpp,
            SourcecodeCppObjcppPreprocessed,
            SourcecodeDtrace,
            SourcecodeExports,
            SourcecodeFortran,
            SourcecodeFortranF77,
            SourcecodeFortranF90,
            SourcecodeGlsl,
            SourcecodeJam,
            SourcecodeJava,
            SourcecodeJavascript,
            SourcecodeLex,
            SourcecodeMake,
            SourcecodeMig,
            SourcecodeNasm,
            SourcecodeOpencl,
            SourcecodePascal,
            SourcecodeRez,
            SourcecodeYacc,
            Text,
            TextCss,
            TextHtmlDocumentation,
            TextMan,
            TextPbxproject,
            TextPlist,
            TextPlistInfo,
            TextPlistScriptSuite,
            TextPlistScriptTerminology,
            TextPlistStrings,
            TextPlistXclangspec,
            TextPlistXcsynspec,
            TextPlistXctxtmacro,
            TextPlistXml,
            TextRtf,
            TextScript,
            TextScriptCsh,
            TextScriptPerl,
            TextScriptPhp,
            TextScriptPython,
            TextScriptRuby,
            TextScriptSh,
            TextScriptWorksheet,
            TextXcconfig,
            TextXml,
            VideoAvi,
            VideoMpeg,
            VideoQuartzComposer,
            VideoQuicktime,
            WrapperApplication,
            WrapperCFBundle,
            WrapperFramework,
            WrapperPBProject
        };

        inline std::string toString(PbxFileType fileType)
        {
            switch (fileType)
            {
                case PbxFileType::ArchiveAr: return "archive.ar";
                case PbxFileType::ArchiveAsdictionary: return "archive.asdictionary";
                case PbxFileType::ArchiveBinhex: return "archive.binhex";
                case PbxFileType::ArchiveEar: return "archive.ear";
                case PbxFileType::ArchiveGzip: return "archive.gzip";
                case PbxFileType::ArchiveJar: return "archive.jar";
                case PbxFileType::ArchiveMacbinary: return "archive.macbinary";
                case PbxFileType::ArchivePpob: return "archive.ppob";
                case PbxFileType::ArchiveRsrc: return "archive.rsrc";
                case PbxFileType::ArchiveStuffit: return "archive.stuffit";
                case PbxFileType::ArchiveTar: return "archive.tar";
                case PbxFileType::ArchiveWar: return "archive.war";
                case PbxFileType::ArchiveZip: return "archive.zip";
                case PbxFileType::AudioAiff: return "audio.aiff";
                case PbxFileType::AudioAu: return "audio.au";
                case PbxFileType::AudioMidi: return "audio.midi";
                case PbxFileType::AudioMp3: return "audio.mp3";
                case PbxFileType::AudioWav: return "audio.wav";
                case PbxFileType::CompiledMachO: return "compiled.mach-o";
                case PbxFileType::CompiledMachOBundle: return "compiled.mach-o.bundle";
                case PbxFileType::CompiledMachOCorefile: return "compiled.mach-o.corefile";
                case PbxFileType::CompiledMachODylib: return "compiled.mach-o.dylib";
                case PbxFileType::CompiledMachOExecutable: return "compiled.mach-o.executable";
                case PbxFileType::CompiledMachOFvmlib: return "compiled.mach-o.fvmlib";
                case PbxFileType::CompiledMachOObjfile: return "compiled.mach-o.objfile";
                case PbxFileType::CompiledMachOPreload: return "compiled.mach-o.preload";
                case PbxFileType::FileBplist: return "file.bplist";
                case PbxFileType::FileXib: return "file.xib";
                case PbxFileType::ImageBmp: return "image.bmp";
                case PbxFileType::ImageGif: return "image.gif";
                case PbxFileType::ImageIcns: return "image.icns";
                case PbxFileType::ImageIco: return "image.ico";
                case PbxFileType::ImageJpeg: return "image.jpeg";
                case PbxFileType::ImagePdf: return "image.pdf";
                case PbxFileType::ImagePict: return "image.pict";
                case PbxFileType::ImagePng: return "image.png";
                case PbxFileType::ImageTiff: return "image.tiff";
                case PbxFileType::PatternProxy: return "pattern.proxy";
                case PbxFileType::SourcecodeAda: return "sourcecode.ada";
                case PbxFileType::SourcecodeApplescript: return "sourcecode.applescript";
                case PbxFileType::SourcecodeAsm: return "sourcecode.asm";
                case PbxFileType::SourcecodeAsmAsm: return "sourcecode.asm.asm";
                case PbxFileType::SourcecodeAsmLlvm: return "sourcecode.asm.llvm";
                case PbxFileType::SourcecodeC: return "sourcecode.c";
                case PbxFileType::SourcecodeCCPreprocessed: return "sourcecode.c.c.preprocessed";
                case PbxFileType::SourcecodeCH: return "sourcecode.c.h";
                case PbxFileType::SourcecodeCObjc: return "sourcecode.c.objc";
                case PbxFileType::SourcecodeCObjcPreprocessed: return "sourcecode.c.objc.preprocessed";
                case PbxFileType::SourcecodeCppCpp: return "sourcecode.cpp.cpp";
                case PbxFileType::SourcecodeCppCppPreprocessed: return "sourcecode.cpp.cpp.preprocessed";
                case PbxFileType::SourcecodeCppH: return "sourcecode.cpp.h";
                case PbxFileType::SourcecodeCppObjcpp: return "sourcecode.cpp.objcpp";
                case PbxFileType::SourcecodeCppObjcppPreprocessed: return "sourcecode.cpp.objcpp.preprocessed";
                case PbxFileType::SourcecodeDtrace: return "sourcecode.dtrace";
                case PbxFileType::SourcecodeExports: return "sourcecode.exports";
                case PbxFileType::SourcecodeFortran: return "sourcecode.fortran";
                case PbxFileType::SourcecodeFortranF77: return "sourcecode.fortran.f77";
                case PbxFileType::SourcecodeFortranF90: return "sourcecode.fortran.f90";
                case PbxFileType::SourcecodeGlsl: return "sourcecode.glsl";
                case PbxFileType::SourcecodeJam: return "sourcecode.jam";
                case PbxFileType::SourcecodeJava: return "sourcecode.java";
                case PbxFileType::SourcecodeJavascript: return "sourcecode.javascript";
                case PbxFileType::SourcecodeLex: return "sourcecode.lex";
                case PbxFileType::SourcecodeMake: return "sourcecode.make";
                case PbxFileType::SourcecodeMig: return "sourcecode.mig";
                case PbxFileType::SourcecodeNasm: return "sourcecode.nasm";
                case PbxFileType::SourcecodeOpencl: return "sourcecode.opencl";
                case PbxFileType::SourcecodePascal: return "sourcecode.pascal";
                case PbxFileType::SourcecodeRez: return "sourcecode.rez";
                case PbxFileType::SourcecodeYacc: return "sourcecode.yacc";
                case PbxFileType::Text: return "text";
                case PbxFileType::TextCss: return "text.css";
                case PbxFileType::TextHtmlDocumentation: return "text.html.documentation";
                case PbxFileType::TextMan: return "text.man";
                case PbxFileType::TextPbxproject: return "text.pbxproject";
                case PbxFileType::TextPlist: return "text.plist";
                case PbxFileType::TextPlistInfo: return "text.plist.info";
                case PbxFileType::TextPlistScriptSuite: return "text.plist.scriptSuite";
                case PbxFileType::TextPlistScriptTerminology: return "text.plist.scriptTerminology";
                case PbxFileType::TextPlistStrings: return "text.plist.strings";
                case PbxFileType::TextPlistXclangspec: return "text.plist.xclangspec";
                case PbxFileType::TextPlistXcsynspec: return "text.plist.xcsynspec";
                case PbxFileType::TextPlistXctxtmacro: return "text.plist.xctxtmacro";
                case PbxFileType::TextPlistXml: return "text.plist.xml";
                case PbxFileType::TextRtf: return "text.rtf";
                case PbxFileType::TextScript: return "text.script";
                case PbxFileType::TextScriptCsh: return "text.script.csh";
                case PbxFileType::TextScriptPerl: return "text.script.perl";
                case PbxFileType::TextScriptPhp: return "text.script.php";
                case PbxFileType::TextScriptPython: return "text.script.python";
                case PbxFileType::TextScriptRuby: return "textScript.ruby";
                case PbxFileType::TextScriptSh: return "text.scriptSh";
                case PbxFileType::TextScriptWorksheet: return "text.script.worksheet";
                case PbxFileType::TextXcconfig: return "text.xcconfig";
                case PbxFileType::TextXml: return "text.xml";
                case PbxFileType::VideoAvi: return "video.avi";
                case PbxFileType::VideoMpeg: return "video.mpeg";
                case PbxFileType::VideoQuartzComposer: return "video.quartz-composer";
                case PbxFileType::VideoQuicktime: return "video.quicktime";
                case PbxFileType::WrapperApplication: return "wrapper.application";
                case PbxFileType::WrapperCFBundle: return "wrapper.cfbundle";
                case PbxFileType::WrapperFramework: return "wrapper.framework";
                case PbxFileType::WrapperPBProject: return "wrapper.pb-project";
            }
        }
    }
}

#endif // OUZEL_XCODE_PBXFILETYPE_HPP
