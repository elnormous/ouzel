// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODEPROJECT_HPP
#define OUZEL_XCODEPROJECT_HPP

#include <fstream>
#include <map>
#include <random>
#include <stack>
#include <type_traits>
#include "OuzelProject.hpp"
#include "storage/FileSystem.hpp"

namespace ouzel
{
    namespace
    {
        class TypeError final: public std::runtime_error
        {
        public:
            explicit TypeError(const std::string& str): std::runtime_error(str) {}
            explicit TypeError(const char* str): std::runtime_error(str) {}
        };

        class RangeError final: public std::runtime_error
        {
        public:
            explicit RangeError(const std::string& str): std::runtime_error(str) {}
            explicit RangeError(const char* str): std::runtime_error(str) {}
        };

        class Plist final
        {
        public:
            class Value final
            {
            public:
                enum class Type
                {
                    Dictionary,
                    Array,
                    String,
                    Number,
                    Data
                };

                using Dictionary = std::map<std::string, Value>;
                using Array = std::vector<Value>;
                using Data = std::vector<uint8_t>;

                Value() = default;
                Value(const Dictionary& value):type{Type::Dictionary}, dictionaryValue{value} {}
                Value(const Array& value):type{Type::Array}, arrayValue{value} {}
                template <class T, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr>
                Value(T value):type{Type::Number}, numberValue{static_cast<double>(value)} {}
                Value(const std::string& value):type{Type::String}, stringValue{value} {}
                Value(const char* value):type{Type::String}, stringValue{value} {}
                Value(const Data& value):type{Type::Data}, dataValue{value} {}

                Value& operator=(const Dictionary& value)
                {
                    type = Type::Dictionary;
                    dictionaryValue = value;
                    return *this;
                }

                Value& operator=(const Array& value)
                {
                    type = Type::Array;
                    arrayValue = value;
                    return *this;
                }

                template <class T, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr>
                Value& operator=(T value)
                {
                    type = Type::Number;
                    numberValue = static_cast<double>(value);
                    return *this;
                }

                Value& operator=(const std::string& value)
                {
                    type = Type::String;
                    stringValue = value;
                    return *this;
                }

                Value& operator=(const char* value)
                {
                    type = Type::String;
                    stringValue = value;
                    return *this;
                }

                Value& operator=(const Data& value)
                {
                    type = Type::Data;
                    dataValue = value;
                    return *this;
                }

                Type getType() const noexcept { return type; }

                template <typename T, typename std::enable_if<std::is_same<T, std::string>::value>::type* = nullptr>
                inline T& as() noexcept
                {
                    type = Type::String;
                    return stringValue;
                }

                template <typename T, typename std::enable_if<std::is_same<T, std::string>::value>::type* = nullptr>
                inline const T& as() const
                {
                    if (type != Type::String) throw TypeError("Wrong type");
                    return stringValue;
                }

                template <typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr>
                inline T as() const
                {
                    if (type != Type::Number)
                        throw TypeError("Wrong type");

                    return static_cast<T>(numberValue);
                }

                template <typename T, typename std::enable_if<std::is_same<T, Dictionary>::value>::type* = nullptr>
                inline T& as() noexcept
                {
                    type = Type::Dictionary;
                    return dictionaryValue;
                }

                template <typename T, typename std::enable_if<std::is_same<T, Dictionary>::value>::type* = nullptr>
                inline const T& as() const
                {
                    if (type != Type::Dictionary) throw TypeError("Wrong type");
                    return dictionaryValue;
                }

                template <typename T, typename std::enable_if<std::is_same<T, Array>::value>::type* = nullptr>
                inline T& as() noexcept
                {
                    type = Type::Array;
                    return arrayValue;
                }

                template <typename T, typename std::enable_if<std::is_same<T, Array>::value>::type* = nullptr>
                inline const T& as() const
                {
                    if (type != Type::Array) throw TypeError("Wrong type");
                    return arrayValue;
                }

                template <typename T, typename std::enable_if<std::is_same<T, Data>::value>::type* = nullptr>
                inline const T& as() const
                {
                    if (type != Type::Data) throw TypeError("Wrong type");
                    return dataValue;
                }

                Array::iterator begin()
                {
                    if (type != Type::Array) throw TypeError("Wrong type");
                    return arrayValue.begin();
                }

                Array::iterator end()
                {
                    if (type != Type::Array) throw TypeError("Wrong type");
                    return arrayValue.end();
                }

                Array::const_iterator begin() const
                {
                    if (type != Type::Array) throw TypeError("Wrong type");
                    return arrayValue.begin();
                }

                Array::const_iterator end() const
                {
                    if (type != Type::Array) throw TypeError("Wrong type");
                    return arrayValue.end();
                }

                inline auto hasMember(const std::string& member) const
                {
                    if (type != Type::Dictionary) throw TypeError("Wrong type");
                    return dictionaryValue.find(member) != dictionaryValue.end();
                }

                inline Value& operator[](const std::string& member)
                {
                    type = Type::Dictionary;
                    return dictionaryValue[member];
                }

                inline const Value& operator[](const std::string& member) const
                {
                    if (type != Type::Dictionary) throw TypeError("Wrong type");

                    auto i = dictionaryValue.find(member);
                    if (i != dictionaryValue.end())
                        return i->second;
                    else
                        throw RangeError("Member does not exist");
                }

                inline Value& operator[](std::size_t index)
                {
                    type = Type::Array;
                    if (index >= arrayValue.size()) arrayValue.resize(index + 1);
                    return arrayValue[index];
                }

                inline const Value& operator[](std::size_t index) const
                {
                    if (type != Type::Array) throw TypeError("Wrong type");

                    if (index < arrayValue.size())
                        return arrayValue[index];
                    else
                        throw RangeError("Index out of range");
                }

                inline auto getSize() const
                {
                    if (type != Type::Array) throw TypeError("Wrong type");
                    return arrayValue.size();
                }

                inline void resize(std::size_t size)
                {
                    if (type != Type::Array) throw TypeError("Wrong type");
                    arrayValue.resize(size);
                }

                inline void pushBack(uint8_t value)
                {
                    if (type != Type::Data) throw TypeError("Wrong type");
                    dataValue.push_back(value);
                }

                inline void pushBack(const Value& value)
                {
                    if (type != Type::Array) throw TypeError("Wrong type");
                    arrayValue.push_back(value);
                }

            private:
                Type type = Type::Dictionary;
                Dictionary dictionaryValue;
                Array arrayValue;
                std::string stringValue;
                double numberValue;
                Data dataValue;
            };

            std::string encode() const
            {
                std::string result = "// !$*UTF8*$!\n";
                return encodeValue(root, result);
            }

            Value root;

        private:
            static std::string& encodeString(const std::string& s,
                                             std::string& result)
            {
                if (!s.empty())
                {
                    bool hasSpecialChars = false;
                    for (auto c : s)
                        if ((c < 'a' || c > 'z') &&
                            (c < 'A' || c > 'Z') &&
                            (c < '0' || c > '9') &&
                            c != '_' && c != '$' && c != '+' && c != '/' &&
                            c != ':' && c != '.' && c != '-')
                        {
                            hasSpecialChars = true;
                            break;
                        }

                    if (hasSpecialChars) result.push_back('"');
                    for (const auto c : s)
                    {
                        if (c == '"') result += '\\';
                        result += c;
                    }
                    if (hasSpecialChars) result.push_back('"');
                }
                else
                    result += "\"\"";

                return result;
            }

            static std::string& encodeValue(const Value& value,
                                            std::string& result)
            {
                switch (value.getType())
                {
                    case Value::Type::Dictionary:
                    {
                        result += '{';
                        for (const auto& entry : value.as<Value::Dictionary>())
                        {
                            encodeString(entry.first, result);
                            result += '=';
                            encodeValue(entry.second, result);
                            result += ';';
                        }
                        result += '}';
                        break;
                    }
                    case Value::Type::Array:
                    {
                        result += '(';
                        for (const auto& child : value.as<Value::Array>())
                        {
                            encodeValue(child, result);
                            result += ',';
                        }
                        result += ')';
                        break;
                    }
                    case Value::Type::String:
                        encodeString(value.as<std::string>(), result);
                        break;
                    case Value::Type::Number:
                    {
                        double number = value.as<double>();
                        double intPart;
                        if (std::modf(number, &intPart) != 0.0)
                            result += std::to_string(number);
                        else
                            result += std::to_string(static_cast<int64_t>(intPart));
                        break;
                    }
                    case Value::Type::Data:
                        result += '<';
                        for (const auto b : value.as<Value::Data>())
                        {
                            constexpr char digits[] = "0123456789ABCDEF";
                            result += digits[(b >> 4) & 0x0F];
                            result += digits[(b >> 0) & 0x0F];
                        }
                        result += '>';
                        break;
                };

                return result;
            }
        };

        std::string generateId()
        {
            std::random_device randomDevice;
            std::mt19937 randomEngine(randomDevice());
            std::uniform_int_distribution<uint32_t> distribution(0, 255);

            std::string result;
            result.resize(2 * 12);

            for (size_t i = 0; i < 12; ++i)
            {
                constexpr char digits[] = "0123456789ABCDEF";
                auto n = distribution(randomEngine);
                result[i * 2 + 0] = digits[(n >> 4) & 0x0F];
                result[i * 2 + 1] = digits[(n >> 0) & 0x0F];
            }

            return result;
        }

        class Object
        {
        public:
            Object(const std::string& i,
                   const std::string& n): isa{i}, name{n} {}
            virtual ~Object() = default;

            const std::string& getId() const noexcept { return id; }
            const std::string& getIsa() const noexcept { return isa; }
            const std::string& getName() const noexcept { return name; }

        private:
            std::string id = generateId();
            std::string isa;
            std::string name;
        };

        class PbxFileElement: public Object
        {
        public:
            PbxFileElement(const std::string& i,
                           const std::string& n):
                Object{i, n} {}
        };

        enum class PbxSourceTree
        {
            Absolute,
            Group,
            SourceRoot,
            BuildProductsDir,
            SdkRoot,
            DeveloperDir
        };

        std::string sourceTreeToString(PbxSourceTree sourceTree)
        {
            switch (sourceTree)
            {
                case PbxSourceTree::Absolute: return "<absolute>";
                case PbxSourceTree::Group: return "<group>";
                case PbxSourceTree::SourceRoot: return "SOURCE_ROOT";
                case PbxSourceTree::BuildProductsDir: return "BUILT_PRODUCTS_DIR";
                case PbxSourceTree::SdkRoot: return "SDKROOT";
                case PbxSourceTree::DeveloperDir: return "DEVELOPER_DIR";
            }
        }

        class PbxFileReference final: public PbxFileElement
        {
        public:
            PbxFileReference(const storage::Path& p, PbxSourceTree tree):
                PbxFileElement{"PBXFileReference", std::string{p}}, path{p}, sourceTree{tree} {}
            const storage::Path& getPath() const noexcept { return path; }

            Plist::Value getValue() const
            {
                const auto extension = path.getExtension();
                const std::string fileType = (sourceTree == PbxSourceTree::BuildProductsDir) ?
                    "wrapper.application" :
                    extension == "plist" ? "text.plist.xml" : "sourcecode.cpp." + extension;

                return Plist::Value::Dictionary{
                    {"isa", getIsa()},
                    {"explicitFileType", fileType},
                    {"includeInIndex", 0},
                    {"path", std::string(path)},
                    {"sourceTree", sourceTreeToString(sourceTree)}
                };
            }

        private:
            storage::Path path;
            PbxSourceTree sourceTree;
        };

        class PbxBuildFile final: public Object
        {
        public:
            PbxBuildFile(const PbxFileReference& ref):
                Object{"PBXBuildFile", ref.getName()}, fileRef{ref} {}

            Plist::Value getValue() const
            {
                return Plist::Value::Dictionary{
                    {"isa", getIsa()},
                    {"fileRef", fileRef.getId()}
                };
            }

        private:
            const PbxFileReference& fileRef;
        };

        class PbxGroup final: public PbxFileElement
        {
        public:
            PbxGroup(const std::string& n,
                     const storage::Path& p,
                     const std::vector<const PbxFileElement*>& c,
                     PbxSourceTree tree):
                PbxFileElement{"PBXGroup", n},
                path{p},
                children{c},
                sourceTree{tree} {}

            Plist::Value getValue() const
            {
                auto result = Plist::Value::Dictionary{
                    {"isa", getIsa()},
                    {"children", Plist::Value::Array{}},
                    {"sourceTree", sourceTreeToString(sourceTree)}
                };

                if (!std::string(path).empty())
                    result["path"] = std::string(path);
                else if (!getName().empty())
                    result["name"] = getName();

                for (auto child : children)
                    result["children"].pushBack(child->getId());

                return result;
            }

        private:
            storage::Path path;
            std::vector<const PbxFileElement*> children;
            PbxSourceTree sourceTree;
        };

        class XcBuildConfiguration final: public Object
        {
        public:
            XcBuildConfiguration(const std::string& n,
                                 const std::string& p = ""):
                Object{"XCBuildConfiguration", n},
                productName{p} {}

            Plist::Value getValue() const
            {
                auto result = Plist::Value::Dictionary{
                    {"isa", getIsa()},
                    {"buildSettings", Plist::Value::Dictionary{}},
                    {"name", getName()}
                };

                if (!productName.empty())
                    result["PRODUCT_NAME"] = productName;

                return result;
            }

        private:
            std::string productName;
        };

        class XcConfigurationList final: public Object
        {
        public:
            XcConfigurationList(const std::string& n,
                                const std::vector<const XcBuildConfiguration*>& c,
                                const XcBuildConfiguration& defaultConfig):
                Object{"XCConfigurationList", n},
                configurations{c},
                defaultConfiguration{defaultConfig} {}

            Plist::Value getValue() const
            {
                auto result = Plist::Value::Dictionary{
                    {"isa", getIsa()},
                    {"buildConfigurations", Plist::Value::Array{}},
                    {"defaultConfigurationIsVisible", 0},
                    {"defaultConfigurationName", defaultConfiguration.getName()},
                };

                for (auto configuration : configurations)
                    result["buildConfigurations"].pushBack(configuration->getId());

                return result;
            }

        private:
            std::vector<const XcBuildConfiguration*> configurations;
            const XcBuildConfiguration& defaultConfiguration;
        };

        class PbxBuildPhase: public Object
        {
        public:
            PbxBuildPhase(const std::string& i,
                          const std::string& n):
                Object{i, n} {}
        };

        class PbxSourcesBuildPhase final: public PbxBuildPhase
        {
        public:
            PbxSourcesBuildPhase(const std::string& n,
                                 const std::vector<const PbxBuildFile*>& f):
                PbxBuildPhase{"PBXSourcesBuildPhase", n},
                files{f} {}

            Plist::Value getValue() const
            {
                auto result = Plist::Value::Dictionary{
                    {"isa", getIsa()},
                    {"buildActionMask", 2147483647},
                    {"files", Plist::Value::Array{}},
                    {"runOnlyForDeploymentPostprocessing", 0},
                };

                for (auto file : files)
                    result["files"].pushBack(file->getId());

                return result;
            }

        private:
            std::vector<const PbxBuildFile*> files;
        };

        class PbxTarget: public Object
        {
        public:
            PbxTarget(const std::string& i,
                      const std::string& n):
                Object{i, n} {}
        };

        class PbxNativeTarget final: public PbxTarget
        {
        public:
            PbxNativeTarget(const std::string& n,
                            const XcConfigurationList& buildConfigList,
                            const std::vector<const PbxBuildPhase*>& phases,
                            const PbxFileReference& product):
                PbxTarget{"PBXNativeTarget", n},
                buildConfigurationList{buildConfigList},
                buildPhases{phases},
                productReference{product} {}

            Plist::Value getValue() const
            {
                auto result = Plist::Value::Dictionary{
                    {"isa", getIsa()},
                    {"buildConfigurationList", buildConfigurationList.getId()},
                    {"buildPhases", Plist::Value::Array{}},
                    {"buildRules", Plist::Value::Array{}},
                    {"dependencies", Plist::Value::Array{}},
                    {"name", getName()},
                    {"productName", getName()},
                    {"productReference", productReference.getId()},
                    {"productType", "com.apple.product-type.application"}
                };

                for (auto buildPhase : buildPhases)
                    result["buildPhases"].pushBack(buildPhase->getId());

                return result;
            }

        private:
            const XcConfigurationList& buildConfigurationList;
            std::vector<const PbxBuildPhase*> buildPhases;
            const PbxFileReference& productReference;
        };

        class PbxProject final: public Object
        {
        public:
            PbxProject(const std::string& n,
                       const std::string& org,
                       const XcConfigurationList& buildConfigList,
                       const PbxGroup& mainGrp,
                       const PbxGroup& productRefGrp,
                       const std::vector<const PbxTarget*>& t):
                Object{"PBXProject", n},
                organization{org},
                buildConfigurationList{buildConfigList},
                mainGroup(mainGrp),
                productRefGroup(productRefGrp),
                targets{t} {}

            Plist::Value getValue() const
            {
                auto result = Plist::Value::Dictionary{
                    {"isa", getIsa()},
                    {"attributes", Plist::Value::Dictionary{
                        {"LastUpgradeCheck", "0800"},
                        {"ORGANIZATIONNAME", organization}
                    }},
                    {"buildConfigurationList", buildConfigurationList.getId()},
                    {"compatibilityVersion", "Xcode 9.3"},
                    {"developmentRegion", "en"},
                    {"hasScannedForEncodings", 0},
                    {"knownRegions", Plist::Value::Array{"en", "Base"}},
                    {"mainGroup", mainGroup.getId()},
                    {"productRefGroup", productRefGroup.getId()},
                    {"projectDirPath", ""},
                    {"projectRoot", ""},
                    {"targets", Plist::Value::Array{}}
                };

                for (auto target : targets)
                    result["targets"].pushBack(target->getId());

                return result;
            }

        private:
            std::string organization;
            const XcConfigurationList& buildConfigurationList;
            const PbxGroup& mainGroup;
            const PbxGroup& productRefGroup;
            std::vector<const PbxTarget*> targets;
        };
    }

    class XcodeProject final
    {
    public:
        XcodeProject(const OuzelProject& p):
            project(p)
        {
        }

        void generate()
        {
            auto projectDirectory = project.getDirectoryPath() / storage::Path{project.getName() + ".xcodeproj"};
            auto projectDirectoryType = projectDirectory.getType();

            if (projectDirectoryType == storage::Path::Type::NotFound)
            {
                storage::FileSystem::createDirectory(projectDirectory);
            }
            else if (projectDirectoryType != storage::Path::Type::Directory)
            {
                storage::FileSystem::deleteFile(projectDirectory);
                storage::FileSystem::createDirectory(projectDirectory);
            }

            auto projectFile = projectDirectory / storage::Path{"project.pbxproj"};

            std::vector<const PbxBuildFile*> buildFiles;
            std::vector<const PbxFileElement*> sourceFiles;
            std::vector<const PbxFileElement*> productFiles;
            std::vector<const PbxFileReference*> fileReferences;

            const auto& productFile = create<PbxFileReference>(storage::Path{project.getName() + ".app"}, PbxSourceTree::BuildProductsDir);
            fileReferences.push_back(&productFile);
            productFiles.push_back(&productFile);

            for (const auto& sourceFile : project.getSourceFiles())
            {
                const auto& fileReference = create<PbxFileReference>(sourceFile, PbxSourceTree::Group);
                const auto& buildFile = create<PbxBuildFile>(fileReference);

                fileReferences.push_back(&fileReference);
                buildFiles.push_back(&buildFile);
                sourceFiles.push_back(&fileReference);
            }

            std::vector<const PbxGroup*> groups;

            const auto& productRefGroup = create<PbxGroup>("Products", storage::Path{}, productFiles, PbxSourceTree::Group);
            const auto& sourceGroup = create<PbxGroup>("src", storage::Path{"src"}, sourceFiles, PbxSourceTree::Group);
            const auto& mainGroup = create<PbxGroup>("", storage::Path{}, std::vector<const PbxFileElement*>{&productRefGroup, &sourceGroup}, PbxSourceTree::Group);
            groups.push_back(&mainGroup);
            groups.push_back(&productRefGroup);
            groups.push_back(&sourceGroup);

            std::vector<const XcBuildConfiguration*> configurations;

            const auto& debugConfiguration = create<XcBuildConfiguration>("Debug");
            configurations.push_back(&debugConfiguration);

            const auto& releaseConfiguration = create<XcBuildConfiguration>("Release");
            configurations.push_back(&releaseConfiguration);

            std::vector<const XcConfigurationList*> configurationLists;
            const auto& projectConfigurationList = create<XcConfigurationList>("Project",
                                                                               std::vector<const XcBuildConfiguration*>{&debugConfiguration, &releaseConfiguration},
                                                                               releaseConfiguration);
            configurationLists.push_back(&projectConfigurationList);

            std::vector<const PbxSourcesBuildPhase*> sourcesBuildPhases;

            std::vector<const PbxTarget*> targets;
            std::vector<const PbxNativeTarget*> nativeTargets;

            for (const auto platform : project.getPlatforms())
            {
                // TODO: do it for all platforms
                if (platform == Platform::MacOs)
                {
                    const auto& targetDebugConfiguration = create<XcBuildConfiguration>("Debug", project.getName());
                    configurations.push_back(&targetDebugConfiguration);

                    const auto& targetReleaseConfiguration = create<XcBuildConfiguration>("Release", project.getName());
                    configurations.push_back(&targetReleaseConfiguration);

                    const auto& targetConfigurationList = create<XcConfigurationList>("Target",
                                                                                      std::vector<const XcBuildConfiguration*>{&targetDebugConfiguration, &targetReleaseConfiguration},
                                                                                      targetReleaseConfiguration);
                    configurationLists.push_back(&targetConfigurationList);

                    const auto& buildSourcesPhase = create<PbxSourcesBuildPhase>("Sources", buildFiles);
                    sourcesBuildPhases.push_back(&buildSourcesPhase);

                    const auto& nativeTarget = create<PbxNativeTarget>(project.getName() + " macOS",
                                                                       targetConfigurationList,
                                                                       std::vector<const PbxBuildPhase*>{&buildSourcesPhase},
                                                                       productFile);
                    nativeTargets.push_back(&nativeTarget);
                    targets.push_back(&nativeTarget);
                }
            }

            const auto& pbxProject = create<PbxProject>("Project object",
                                                        project.getOrganization(),
                                                        projectConfigurationList,
                                                        mainGroup,
                                                        productRefGroup,
                                                        targets);

            Plist plist;
            plist.root = Plist::Value::Dictionary{
                {"archiveVersion", 1},
                {"classes", Plist::Value::Dictionary{}},
                {"objectVersion", 50},
                {"objects", Plist::Value::Dictionary{}}
            };

            // PBXBuildFile section
            for (const auto buildFile : buildFiles)
                plist.root["objects"][buildFile->getId()] = buildFile->getValue();

            // PBXFileReference section
            for (const auto fileReference : fileReferences)
                plist.root["objects"][fileReference->getId()] = fileReference->getValue();

            // frameworks buid phase
            // PBXFrameworksBuildPhase section
            // TODO

            // PBXGroup section
            for (const auto group : groups)
                plist.root["objects"][group->getId()] = group->getValue();

            // PBXNativeTarget section
            for (const auto nativeTarget : nativeTargets)
                plist.root["objects"][nativeTarget->getId()] = nativeTarget->getValue();

            // PBXProject section
            plist.root["objects"][pbxProject.getId()] = pbxProject.getValue();

            // resource build phases
            // PBXResourcesBuildPhase section
            // TODO

            // source file build phase
            // PBXSourcesBuildPhase section
            for (const auto sourcesBuildPhase : sourcesBuildPhases)
                plist.root["objects"][sourcesBuildPhase->getId()] = sourcesBuildPhase->getValue();

            // configurations
            // XCBuildConfiguration section
            for (const auto configuration : configurations)
                plist.root["objects"][configuration->getId()] = configuration->getValue();

            // XCConfigurationList section
            for (const auto configurationList : configurationLists)
                plist.root["objects"][configurationList->getId()] = configurationList->getValue();

            plist.root["rootObject"] = pbxProject.getId();

            std::ofstream file(projectFile, std::ios::trunc);
            file << plist.encode();
        }

    private:
        template <class T, class ...Args>
        T& create(Args&&... args)
        {
            T* result;
            objects.push_back(std::unique_ptr<Object>(result = new T(std::forward<Args>(args)...)));
            return *result;
        }

        const OuzelProject& project;
        std::vector<std::unique_ptr<Object>> objects;
    };
}

#endif // OUZEL_XCODEPROJECT_HPP
