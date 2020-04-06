// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_PBXFILETYPE_HPP
#define OUZEL_XCODE_PBXFILETYPE_HPP

#include <string>

namespace ouzel
{
    namespace xcode
    {
        enum class PBXFileType
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
            Compiled,
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

        inline std::string toString(PBXFileType fileType)
        {
            switch (fileType)
            {
                case PBXFileType::ArchiveAr: return "archive.ar";
                case PBXFileType::ArchiveAsdictionary: return "archive.asdictionary";
                case PBXFileType::ArchiveBinhex: return "archive.binhex";
                case PBXFileType::ArchiveEar: return "archive.ear";
                case PBXFileType::ArchiveGzip: return "archive.gzip";
                case PBXFileType::ArchiveJar: return "archive.jar";
                case PBXFileType::ArchiveMacbinary: return "archive.macbinary";
                case PBXFileType::ArchivePpob: return "archive.ppob";
                case PBXFileType::ArchiveRsrc: return "archive.rsrc";
                case PBXFileType::ArchiveStuffit: return "archive.stuffit";
                case PBXFileType::ArchiveTar: return "archive.tar";
                case PBXFileType::ArchiveWar: return "archive.war";
                case PBXFileType::ArchiveZip: return "archive.zip";
                case PBXFileType::AudioAiff: return "audio.aiff";
                case PBXFileType::AudioAu: return "audio.au";
                case PBXFileType::AudioMidi: return "audio.midi";
                case PBXFileType::AudioMp3: return "audio.mp3";
                case PBXFileType::AudioWav: return "audio.wav";
                case PBXFileType::Compiled: return "compiled";
                case PBXFileType::CompiledMachO: return "compiled.mach-o";
                case PBXFileType::CompiledMachOBundle: return "compiled.mach-o.bundle";
                case PBXFileType::CompiledMachOCorefile: return "compiled.mach-o.corefile";
                case PBXFileType::CompiledMachODylib: return "compiled.mach-o.dylib";
                case PBXFileType::CompiledMachOExecutable: return "compiled.mach-o.executable";
                case PBXFileType::CompiledMachOFvmlib: return "compiled.mach-o.fvmlib";
                case PBXFileType::CompiledMachOObjfile: return "compiled.mach-o.objfile";
                case PBXFileType::CompiledMachOPreload: return "compiled.mach-o.preload";
                case PBXFileType::FileBplist: return "file.bplist";
                case PBXFileType::FileXib: return "file.xib";
                case PBXFileType::ImageBmp: return "image.bmp";
                case PBXFileType::ImageGif: return "image.gif";
                case PBXFileType::ImageIcns: return "image.icns";
                case PBXFileType::ImageIco: return "image.ico";
                case PBXFileType::ImageJpeg: return "image.jpeg";
                case PBXFileType::ImagePdf: return "image.pdf";
                case PBXFileType::ImagePict: return "image.pict";
                case PBXFileType::ImagePng: return "image.png";
                case PBXFileType::ImageTiff: return "image.tiff";
                case PBXFileType::PatternProxy: return "pattern.proxy";
                case PBXFileType::SourcecodeAda: return "sourcecode.ada";
                case PBXFileType::SourcecodeApplescript: return "sourcecode.applescript";
                case PBXFileType::SourcecodeAsm: return "sourcecode.asm";
                case PBXFileType::SourcecodeAsmAsm: return "sourcecode.asm.asm";
                case PBXFileType::SourcecodeAsmLlvm: return "sourcecode.asm.llvm";
                case PBXFileType::SourcecodeC: return "sourcecode.c";
                case PBXFileType::SourcecodeCCPreprocessed: return "sourcecode.c.c.preprocessed";
                case PBXFileType::SourcecodeCH: return "sourcecode.c.h";
                case PBXFileType::SourcecodeCObjc: return "sourcecode.c.objc";
                case PBXFileType::SourcecodeCObjcPreprocessed: return "sourcecode.c.objc.preprocessed";
                case PBXFileType::SourcecodeCppCpp: return "sourcecode.cpp.cpp";
                case PBXFileType::SourcecodeCppCppPreprocessed: return "sourcecode.cpp.cpp.preprocessed";
                case PBXFileType::SourcecodeCppH: return "sourcecode.cpp.h";
                case PBXFileType::SourcecodeCppObjcpp: return "sourcecode.cpp.objcpp";
                case PBXFileType::SourcecodeCppObjcppPreprocessed: return "sourcecode.cpp.objcpp.preprocessed";
                case PBXFileType::SourcecodeDtrace: return "sourcecode.dtrace";
                case PBXFileType::SourcecodeExports: return "sourcecode.exports";
                case PBXFileType::SourcecodeFortran: return "sourcecode.fortran";
                case PBXFileType::SourcecodeFortranF77: return "sourcecode.fortran.f77";
                case PBXFileType::SourcecodeFortranF90: return "sourcecode.fortran.f90";
                case PBXFileType::SourcecodeGlsl: return "sourcecode.glsl";
                case PBXFileType::SourcecodeJam: return "sourcecode.jam";
                case PBXFileType::SourcecodeJava: return "sourcecode.java";
                case PBXFileType::SourcecodeJavascript: return "sourcecode.javascript";
                case PBXFileType::SourcecodeLex: return "sourcecode.lex";
                case PBXFileType::SourcecodeMake: return "sourcecode.make";
                case PBXFileType::SourcecodeMig: return "sourcecode.mig";
                case PBXFileType::SourcecodeNasm: return "sourcecode.nasm";
                case PBXFileType::SourcecodeOpencl: return "sourcecode.opencl";
                case PBXFileType::SourcecodePascal: return "sourcecode.pascal";
                case PBXFileType::SourcecodeRez: return "sourcecode.rez";
                case PBXFileType::SourcecodeYacc: return "sourcecode.yacc";
                case PBXFileType::Text: return "text";
                case PBXFileType::TextCss: return "text.css";
                case PBXFileType::TextHtmlDocumentation: return "text.html.documentation";
                case PBXFileType::TextMan: return "text.man";
                case PBXFileType::TextPbxproject: return "text.pbxproject";
                case PBXFileType::TextPlist: return "text.plist";
                case PBXFileType::TextPlistInfo: return "text.plist.info";
                case PBXFileType::TextPlistScriptSuite: return "text.plist.scriptSuite";
                case PBXFileType::TextPlistScriptTerminology: return "text.plist.scriptTerminology";
                case PBXFileType::TextPlistStrings: return "text.plist.strings";
                case PBXFileType::TextPlistXclangspec: return "text.plist.xclangspec";
                case PBXFileType::TextPlistXcsynspec: return "text.plist.xcsynspec";
                case PBXFileType::TextPlistXctxtmacro: return "text.plist.xctxtmacro";
                case PBXFileType::TextPlistXml: return "text.plist.xml";
                case PBXFileType::TextRtf: return "text.rtf";
                case PBXFileType::TextScript: return "text.script";
                case PBXFileType::TextScriptCsh: return "text.script.csh";
                case PBXFileType::TextScriptPerl: return "text.script.perl";
                case PBXFileType::TextScriptPhp: return "text.script.php";
                case PBXFileType::TextScriptPython: return "text.script.python";
                case PBXFileType::TextScriptRuby: return "textScript.ruby";
                case PBXFileType::TextScriptSh: return "text.scriptSh";
                case PBXFileType::TextScriptWorksheet: return "text.script.worksheet";
                case PBXFileType::TextXcconfig: return "text.xcconfig";
                case PBXFileType::TextXml: return "text.xml";
                case PBXFileType::VideoAvi: return "video.avi";
                case PBXFileType::VideoMpeg: return "video.mpeg";
                case PBXFileType::VideoQuartzComposer: return "video.quartz-composer";
                case PBXFileType::VideoQuicktime: return "video.quicktime";
                case PBXFileType::WrapperApplication: return "wrapper.application";
                case PBXFileType::WrapperCFBundle: return "wrapper.cfbundle";
                case PBXFileType::WrapperFramework: return "wrapper.framework";
                case PBXFileType::WrapperPBProject: return "wrapper.pb-project";
            }
        }
    }
}

#endif // OUZEL_XCODE_PBXFILETYPE_HPP
