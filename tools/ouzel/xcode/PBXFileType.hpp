// Ouzel by Elviss Strazdins

#ifndef OUZEL_XCODE_PBXFILETYPE_HPP
#define OUZEL_XCODE_PBXFILETYPE_HPP

#include <string>

namespace ouzel::xcode
{
    enum class PBXFileType
    {
        archiveAr,
        archiveAsdictionary,
        archiveBinhex,
        archiveEar,
        archiveGzip,
        archiveJar,
        archiveMacbinary,
        archivePpob,
        archiveRsrc,
        archiveStuffit,
        archiveTar,
        archiveWar,
        archiveZip,
        audioAiff,
        audioAu,
        audioMidi,
        audioMp3,
        audioWav,
        compiled,
        compiledMachO,
        compiledMachOBundle,
        compiledMachOCorefile,
        compiledMachODylib,
        compiledMachOExecutable,
        compiledMachOFvmlib,
        compiledMachOObjfile,
        compiledMachOPreload,
        fileBplist,
        fileXib,
        imageBmp,
        imageGif,
        imageIcns,
        imageIco,
        imageJpeg,
        imagePdf,
        imagePict,
        imagePng,
        imageTiff,
        patternProxy,
        sourcecodeAda,
        sourcecodeApplescript,
        sourcecodeAsm,
        sourcecodeAsmAsm,
        sourcecodeAsmLlvm,
        sourcecodeC,
        sourcecodeCCPreprocessed,
        sourcecodeCH,
        sourcecodeCObjc,
        sourcecodeCObjcPreprocessed,
        sourcecodeCppCpp,
        sourcecodeCppCppPreprocessed,
        sourcecodeCppH,
        sourcecodeCppObjcpp,
        sourcecodeCppObjcppPreprocessed,
        sourcecodeDtrace,
        sourcecodeExports,
        sourcecodeFortran,
        sourcecodeFortranF77,
        sourcecodeFortranF90,
        sourcecodeGlsl,
        sourcecodeJam,
        sourcecodeJava,
        sourcecodeJavascript,
        sourcecodeLex,
        sourcecodeMake,
        sourcecodeMig,
        sourcecodeNasm,
        sourcecodeOpencl,
        sourcecodePascal,
        sourcecodeRez,
        sourcecodeYacc,
        text,
        textCss,
        textHtmlDocumentation,
        textMan,
        textPbxproject,
        textPlist,
        textPlistInfo,
        textPlistScriptSuite,
        textPlistScriptTerminology,
        textPlistStrings,
        textPlistXclangspec,
        textPlistXcsynspec,
        textPlistXctxtmacro,
        textPlistXml,
        textRtf,
        textScript,
        textScriptCsh,
        textScriptPerl,
        textScriptPhp,
        textScriptPython,
        textScriptRuby,
        textScriptSh,
        textScriptWorksheet,
        textXcconfig,
        textXml,
        videoAvi,
        videoMpeg,
        videoQuartzComposer,
        videoQuicktime,
        wrapperApplication,
        wrapperCFBundle,
        wrapperFramework,
        wrapperPBProject
    };

    inline std::string toString(PBXFileType fileType)
    {
        switch (fileType)
        {
            case PBXFileType::archiveAr: return "archive.ar";
            case PBXFileType::archiveAsdictionary: return "archive.asdictionary";
            case PBXFileType::archiveBinhex: return "archive.binhex";
            case PBXFileType::archiveEar: return "archive.ear";
            case PBXFileType::archiveGzip: return "archive.gzip";
            case PBXFileType::archiveJar: return "archive.jar";
            case PBXFileType::archiveMacbinary: return "archive.macbinary";
            case PBXFileType::archivePpob: return "archive.ppob";
            case PBXFileType::archiveRsrc: return "archive.rsrc";
            case PBXFileType::archiveStuffit: return "archive.stuffit";
            case PBXFileType::archiveTar: return "archive.tar";
            case PBXFileType::archiveWar: return "archive.war";
            case PBXFileType::archiveZip: return "archive.zip";
            case PBXFileType::audioAiff: return "audio.aiff";
            case PBXFileType::audioAu: return "audio.au";
            case PBXFileType::audioMidi: return "audio.midi";
            case PBXFileType::audioMp3: return "audio.mp3";
            case PBXFileType::audioWav: return "audio.wav";
            case PBXFileType::compiled: return "compiled";
            case PBXFileType::compiledMachO: return "compiled.mach-o";
            case PBXFileType::compiledMachOBundle: return "compiled.mach-o.bundle";
            case PBXFileType::compiledMachOCorefile: return "compiled.mach-o.corefile";
            case PBXFileType::compiledMachODylib: return "compiled.mach-o.dylib";
            case PBXFileType::compiledMachOExecutable: return "compiled.mach-o.executable";
            case PBXFileType::compiledMachOFvmlib: return "compiled.mach-o.fvmlib";
            case PBXFileType::compiledMachOObjfile: return "compiled.mach-o.objfile";
            case PBXFileType::compiledMachOPreload: return "compiled.mach-o.preload";
            case PBXFileType::fileBplist: return "file.bplist";
            case PBXFileType::fileXib: return "file.xib";
            case PBXFileType::imageBmp: return "image.bmp";
            case PBXFileType::imageGif: return "image.gif";
            case PBXFileType::imageIcns: return "image.icns";
            case PBXFileType::imageIco: return "image.ico";
            case PBXFileType::imageJpeg: return "image.jpeg";
            case PBXFileType::imagePdf: return "image.pdf";
            case PBXFileType::imagePict: return "image.pict";
            case PBXFileType::imagePng: return "image.png";
            case PBXFileType::imageTiff: return "image.tiff";
            case PBXFileType::patternProxy: return "pattern.proxy";
            case PBXFileType::sourcecodeAda: return "sourcecode.ada";
            case PBXFileType::sourcecodeApplescript: return "sourcecode.applescript";
            case PBXFileType::sourcecodeAsm: return "sourcecode.asm";
            case PBXFileType::sourcecodeAsmAsm: return "sourcecode.asm.asm";
            case PBXFileType::sourcecodeAsmLlvm: return "sourcecode.asm.llvm";
            case PBXFileType::sourcecodeC: return "sourcecode.c";
            case PBXFileType::sourcecodeCCPreprocessed: return "sourcecode.c.c.preprocessed";
            case PBXFileType::sourcecodeCH: return "sourcecode.c.h";
            case PBXFileType::sourcecodeCObjc: return "sourcecode.c.objc";
            case PBXFileType::sourcecodeCObjcPreprocessed: return "sourcecode.c.objc.preprocessed";
            case PBXFileType::sourcecodeCppCpp: return "sourcecode.cpp.cpp";
            case PBXFileType::sourcecodeCppCppPreprocessed: return "sourcecode.cpp.cpp.preprocessed";
            case PBXFileType::sourcecodeCppH: return "sourcecode.cpp.h";
            case PBXFileType::sourcecodeCppObjcpp: return "sourcecode.cpp.objcpp";
            case PBXFileType::sourcecodeCppObjcppPreprocessed: return "sourcecode.cpp.objcpp.preprocessed";
            case PBXFileType::sourcecodeDtrace: return "sourcecode.dtrace";
            case PBXFileType::sourcecodeExports: return "sourcecode.exports";
            case PBXFileType::sourcecodeFortran: return "sourcecode.fortran";
            case PBXFileType::sourcecodeFortranF77: return "sourcecode.fortran.f77";
            case PBXFileType::sourcecodeFortranF90: return "sourcecode.fortran.f90";
            case PBXFileType::sourcecodeGlsl: return "sourcecode.glsl";
            case PBXFileType::sourcecodeJam: return "sourcecode.jam";
            case PBXFileType::sourcecodeJava: return "sourcecode.java";
            case PBXFileType::sourcecodeJavascript: return "sourcecode.javascript";
            case PBXFileType::sourcecodeLex: return "sourcecode.lex";
            case PBXFileType::sourcecodeMake: return "sourcecode.make";
            case PBXFileType::sourcecodeMig: return "sourcecode.mig";
            case PBXFileType::sourcecodeNasm: return "sourcecode.nasm";
            case PBXFileType::sourcecodeOpencl: return "sourcecode.opencl";
            case PBXFileType::sourcecodePascal: return "sourcecode.pascal";
            case PBXFileType::sourcecodeRez: return "sourcecode.rez";
            case PBXFileType::sourcecodeYacc: return "sourcecode.yacc";
            case PBXFileType::text: return "text";
            case PBXFileType::textCss: return "text.css";
            case PBXFileType::textHtmlDocumentation: return "text.html.documentation";
            case PBXFileType::textMan: return "text.man";
            case PBXFileType::textPbxproject: return "text.pbxproject";
            case PBXFileType::textPlist: return "text.plist";
            case PBXFileType::textPlistInfo: return "text.plist.info";
            case PBXFileType::textPlistScriptSuite: return "text.plist.scriptSuite";
            case PBXFileType::textPlistScriptTerminology: return "text.plist.scriptTerminology";
            case PBXFileType::textPlistStrings: return "text.plist.strings";
            case PBXFileType::textPlistXclangspec: return "text.plist.xclangspec";
            case PBXFileType::textPlistXcsynspec: return "text.plist.xcsynspec";
            case PBXFileType::textPlistXctxtmacro: return "text.plist.xctxtmacro";
            case PBXFileType::textPlistXml: return "text.plist.xml";
            case PBXFileType::textRtf: return "text.rtf";
            case PBXFileType::textScript: return "text.script";
            case PBXFileType::textScriptCsh: return "text.script.csh";
            case PBXFileType::textScriptPerl: return "text.script.perl";
            case PBXFileType::textScriptPhp: return "text.script.php";
            case PBXFileType::textScriptPython: return "text.script.python";
            case PBXFileType::textScriptRuby: return "textScript.ruby";
            case PBXFileType::textScriptSh: return "text.scriptSh";
            case PBXFileType::textScriptWorksheet: return "text.script.worksheet";
            case PBXFileType::textXcconfig: return "text.xcconfig";
            case PBXFileType::textXml: return "text.xml";
            case PBXFileType::videoAvi: return "video.avi";
            case PBXFileType::videoMpeg: return "video.mpeg";
            case PBXFileType::videoQuartzComposer: return "video.quartz-composer";
            case PBXFileType::videoQuicktime: return "video.quicktime";
            case PBXFileType::wrapperApplication: return "wrapper.application";
            case PBXFileType::wrapperCFBundle: return "wrapper.cfbundle";
            case PBXFileType::wrapperFramework: return "wrapper.framework";
            case PBXFileType::wrapperPBProject: return "wrapper.pb-project";
        }

        throw std::runtime_error{"Unsupported file type"};
    }
}

#endif // OUZEL_XCODE_PBXFILETYPE_HPP
