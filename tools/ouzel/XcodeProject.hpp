// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODEPROJECT_HPP
#define OUZEL_XCODEPROJECT_HPP

#include <fstream>
#include <random>
#include "OuzelProject.hpp"
#include "storage/FileSystem.hpp"

namespace ouzel
{
    namespace
    {
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

        std::string formatString(const std::string& s)
        {
            bool hasSpecialChars = false;
            for (auto c : s)
                if ((c < 'a' || c > 'z') &&
                    (c < 'A' || c > 'Z') &&
                    (c < '0' || c > '9'))
                {
                    hasSpecialChars = true;
                    break;
                }

            std::string result;
            if (hasSpecialChars) result.push_back('"');
            result += s;
            if (hasSpecialChars) result.push_back('"');
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

        std::string formatReference(const Object& s)
        {
            std::string result = s.getId();
            if (!s.getName().empty())
                result += " /* " + s.getName() + " */";
            return result;
        }

        class PbxFileReference final: public Object
        {
        public:
            enum class Type
            {
                Build,
                Product
            };

            PbxFileReference(const storage::Path& p, Type t = Type::Build):
                Object{"PBXFileReference", std::string{p}}, path{p}, type{t} {}
            const storage::Path& getPath() const noexcept { return path; }

            void output(std::ostream& stream) const
            {
                auto filename = std::string(path);
                auto extension = path.getExtension();
                if (type == Type::Build)
                {
                    auto fileType = extension == "plist" ? "text.plist.xml" : "sourcecode.cpp." + extension;
                    stream << "\t\t" << getId() << " /* " << getName() << " */ = {isa = " << getIsa() << "; lastKnownFileType = " << fileType << "; path = " << formatString(filename) << "; sourceTree = " << formatString("<group>") << "; };\n";
                }
                else if (type == Type::Product)
                {
                    auto fileType = std::string("wrapper.application");
                    stream << "\t\t" << getId() << " /* " << getName() << " */ = {isa = " << getIsa() << "; explicitFileType = " << fileType << "; includeInIndex = " << 0 << "; path = " << formatString(filename) << "; sourceTree = " << "BUILT_PRODUCTS_DIR" << "; };\n";
                }
            }

        private:
            storage::Path path;
            Type type = Type::Build;
        };

        class PbxBuildFile final: public Object
        {
        public:
            PbxBuildFile(const PbxFileReference& ref):
                Object{"PBXBuildFile", ref.getName()}, fileRef{ref} {}

            void output(std::ostream& stream) const
            {
                auto filename = std::string(fileRef.getPath());
                stream << "\t\t" << getId() << " /* " << getName() << " */ = {isa = " << getIsa() << "; fileRef = " << formatReference(fileRef) << "; };\n";
            }

        private:
            const PbxFileReference& fileRef;
        };

        class PbxGroup final: public Object
        {
        public:
            PbxGroup(const std::string& n,
                     const storage::Path& p = {},
                     const std::vector<const Object*>& c = {}):
                Object{"PBXGroup", n}, path{p}, children{c} {}

            void output(std::ostream& stream) const
            {
                stream << "\t\t" << getId() << " ";

                if (!getName().empty())
                    stream << "/* " << getName() << " */ ";

                stream << "= {\n"
                    "\t\t\tisa = " << getIsa() << ";\n"
                    "\t\t\tchildren = (\n";

                for (auto child : children)
                    stream << "\t\t\t\t" << formatReference(*child) << ",\n";

                stream << "\t\t\t);\n";

                if (!std::string(path).empty())
                    stream << "\t\t\tpath = " << formatString(path) << ";\n";
                else if (!getName().empty())
                    stream << "\t\t\tname = " << formatString(getName()) << ";\n";

                stream << "\t\t\tsourceTree = " << formatString("<group>") << ";\n"
                    "\t\t};\n";
            }

        private:
            storage::Path path;
            std::vector<const Object*> children;
        };

        class XcBuildConfiguration final: public Object
        {
        public:
            XcBuildConfiguration(const std::string& n):
                Object{"XCBuildConfiguration", n} {}

            void output(std::ostream& stream) const
            {
                stream << "\t\t" << getId() << " /* " << getName() << " */ = {\n"
                    "\t\t\tisa = " << getIsa() << ";\n"
                    "\t\t\tbuildSettings = {\n"
                    "\t\t\t};\n"
                    "\t\t\tname = " << formatString(getName()) << ";\n"
                    "\t\t};\n";
            }

        private:
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

            void output(std::ostream& stream) const
            {
                stream << "\t\t" << getId() << " /* " << getName() << " */ = {\n"
                    "\t\t\tisa = " << getIsa() << ";\n"
                    "\t\t\tbuildConfigurations = {\n";

                for (auto configuration : configurations)
                    stream << "\t\t\t\t" << formatReference(*configuration) << ",\n";

                stream << "\t\t\t};\n"
                    "\t\t\tdefaultConfigurationIsVisible = " << 0 << ";\n"
                    "\t\t\tdefaultConfigurationName = " << formatString(defaultConfiguration.getName()) << ";\n"
                    "\t\t};\n";
            }

        private:
            std::vector<const XcBuildConfiguration*> configurations;
            const XcBuildConfiguration& defaultConfiguration;
        };

        class PbxSourcesBuildPhase final: public Object
        {
        public:
            PbxSourcesBuildPhase(const std::string& n,
                                 const std::vector<const PbxBuildFile*>& f):
                Object{"PBXSourcesBuildPhase", n},
                files{f} {}

            void output(std::ostream& stream) const
            {
                stream << "\t\t" << getId() << " /* " << getName() << " */ = {\n"
                    "\t\t\tisa = " << getIsa() << ";\n"
                    "\t\t\tbuildActionMask = " << 2147483647 << ";\n"
                    "\t\t\tfiles = (\n";
                for (auto file : files)
                    stream << "\t\t\t\t" << formatReference(*file) << ",\n";
                stream << "\t\t\t);\n"
                    "\t\t\trunOnlyForDeploymentPostprocessing = " << 0 << ";\n";
                stream << "\t\t};\n";
            }

        private:
            std::vector<const PbxBuildFile*> files;
        };

        class PbxNativeTarget final: public Object
        {
        public:
            PbxNativeTarget(const std::string& n,
                            const XcConfigurationList& buildConfigList,
                            const std::vector<const Object*>& phases,
                            const PbxFileReference& product):
                Object{"PBXNativeTarget", n},
                buildConfigurationList{buildConfigList},
                buildPhases{phases},
                productReference{product} {}

            void output(std::ostream& stream) const
            {
                stream << "\t\t" << getId() << " /* " << getName() << " */ = {\n"
                    "\t\t\tisa = " << getIsa() << ";\n"
                    "\t\t\tbuildConfigurationList = " << formatReference(buildConfigurationList) << ";\n"
                    "\t\t\tbuildPhases = (\n";
                for (auto buildPhase : buildPhases)
                    stream << "\t\t\t\t" << formatReference(*buildPhase) << ",\n";
                stream << "\t\t\t);\n"
                    "\t\t\tbuildRules = (\n"
                    "\t\t\t);\n"
                    "\t\t\tdependencies = (\n"
                    "\t\t\t);\n"
                    "\t\t\tname = " << formatString(getName()) << ";\n"
                    "\t\t\tproductName = " << formatString(getName()) << ";\n"
                    "\t\t\tproductReference = " << formatReference(productReference) << ";\n"
                    "\t\t\tproductType = " << "\"com.apple.product-type.application\"" << ";\n";
                stream << "\t\t};\n";
            }

        private:
            const XcConfigurationList& buildConfigurationList;
            std::vector<const Object*> buildPhases;
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
                       const std::vector<const PbxNativeTarget*>& t):
                Object{"PBXProject", n},
                organization{org},
                buildConfigurationList{buildConfigList},
                mainGroup(mainGrp),
                productRefGroup(productRefGrp),
                targets{t} {}

            void output(std::ostream& stream) const
            {
                stream << "\t\t" << getId() << " /* " << getName() << " */ = {\n"
                    "\t\t\tisa = " << getIsa() << ";\n"
                    "\t\t\tattributes = {\n"
                    "\t\t\t\tLastUpgradeCheck = " << "0800" << ";\n"
                    "\t\t\t\tORGANIZATIONNAME = " << formatString(organization) << ";\n";
                stream << "\t\t\t};\n"
                    "\t\t\tbuildConfigurationList = " << buildConfigurationList.getId() << " /* " << buildConfigurationList.getName() << " */;\n"
                    "\t\t\tcompatibilityVersion = " << "\"Xcode 9.3\"" << ";\n"
                    "\t\t\tdevelopmentRegion = " << "en" << ";\n"
                    "\t\t\thasScannedForEncodings = " << 0 << ";\n"
                    "\t\t\tknownRegions = (\n"
                    "\t\t\t\ten,\n"
                    "\t\t\t\tBase,\n"
                    "\t\t\t);\n"
                    "\t\t\tmainGroup = " << formatReference(mainGroup) << ";\n"
                    "\t\t\tproductRefGroup = " << formatReference(productRefGroup) << ";\n"
                    "\t\t\tprojectDirPath = " << "\"\"" << ";\n"
                    "\t\t\tprojectRoot = " << "\"\"" << ";\n"
                    "\t\t\ttargets = (\n";

                for (auto target : targets)
                    stream << "\t\t\t\t" << formatReference(*target) << ",\n";

                stream << "\t\t\t);\n"
                    "\t\t};\n";
            }

        private:
            std::string organization;
            const XcConfigurationList& buildConfigurationList;
            const PbxGroup& mainGroup;
            const PbxGroup& productRefGroup;
            std::vector<const PbxNativeTarget*> targets;
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

            std::vector<const PbxGroup*> groups;

            const auto& productRefGroup = create<PbxGroup>("Products");
            const auto& sourceGroup = create<PbxGroup>("src", storage::Path{"src"});
            const auto& mainGroup = create<PbxGroup>("", storage::Path{}, std::vector<const Object*>{&productRefGroup, &sourceGroup});
            groups.push_back(&mainGroup);
            groups.push_back(&productRefGroup);
            groups.push_back(&sourceGroup);

            std::vector<const PbxBuildFile*> buildFiles;
            std::vector<const PbxFileReference*> fileReferences;

            const auto& productFile = create<PbxFileReference>(storage::Path{project.getName() + ".app"}, PbxFileReference::Type::Product);
            fileReferences.push_back(&productFile);

            for (const auto& sourceFile : project.getSourceFiles())
            {
                const auto& fileReference = create<PbxFileReference>(sourceFile);
                const auto& buildFile = create<PbxBuildFile>(fileReference);

                fileReferences.push_back(&fileReference);
                buildFiles.push_back(&buildFile);
            }

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

            std::vector<const PbxNativeTarget*> nativeTargets;
            // TODO: for each architecture
            {
                const auto& targetDebugConfiguration = create<XcBuildConfiguration>("Debug");
                configurations.push_back(&targetDebugConfiguration);

                const auto& targetReleaseConfiguration = create<XcBuildConfiguration>("Release");
                configurations.push_back(&targetReleaseConfiguration);

                const auto& targetConfigurationList = create<XcConfigurationList>("Target",
                                                                                  std::vector<const XcBuildConfiguration*>{&targetDebugConfiguration, &targetReleaseConfiguration},
                                                                                  targetReleaseConfiguration);
                configurationLists.push_back(&targetConfigurationList);

                const auto& buildSourcesPhase = create<PbxSourcesBuildPhase>("Sources", buildFiles);
                sourcesBuildPhases.push_back(&buildSourcesPhase);

                const auto& nativeTarget = create<PbxNativeTarget>(project.getName(),
                                                                   targetConfigurationList,
                                                                   std::vector<const Object*>{&buildSourcesPhase},
                                                                   productFile);
                nativeTargets.push_back(&nativeTarget);
            }

            PbxProject pbxProject{
                "Project object",
                project.getOrganization(),
                projectConfigurationList,
                mainGroup,
                productRefGroup,
                nativeTargets
            };

            std::ofstream file(projectFile, std::ios::trunc);
            file << "// !$*UTF8*$!\n"
            "{\n"
            "\tarchiveVersion = 1;\n"
            "\tclasses = {\n"
            "\t};\n"
            "\tobjectVersion = 46;\n"
            "\tobjects = {\n";

            file << "\n/* Begin PBXBuildFile section */\n";
            for (const auto buildFile : buildFiles)
                buildFile->output(file);
            file << "/* End PBXBuildFile section */\n";

            file << "\n/* Begin PBXFileReference section */\n";
            for (const auto fileReference : fileReferences)
                fileReference->output(file);
            file << "/* End PBXFileReference section */\n";

            // frameworks buid phase
            //file << "\n/* Begin PBXFrameworksBuildPhase section */\n";
            // TODO
            //file << "/* End PBXFrameworksBuildPhase section */\n";

            file << "\n/* Begin PBXGroup section */\n";
            for (const auto group : groups)
                group->output(file);
            file << "/* End PBXGroup section */\n";

            file << "\n/* Begin PBXNativeTarget section */\n";
            for (const auto nativeTarget : nativeTargets)
                nativeTarget->output(file);
            file << "/* End PBXNativeTarget section */\n";

            file << "\n/* Begin PBXProject section */\n";
            pbxProject.output(file);
            file << "/* End PBXProject section */\n";

            // resource build phases
            //file << "\n/* Begin PBXResourcesBuildPhase section */\n";
            // TODO
            //file << "/* End PBXResourcesBuildPhase section */\n";

            // source file build phase
            file << "\n/* Begin PBXSourcesBuildPhase section */\n";
            for (const auto sourcesBuildPhase : sourcesBuildPhases)
                sourcesBuildPhase->output(file);
            file << "/* End PBXSourcesBuildPhase section */\n";

            // configurations
            file << "\n/* Begin XCBuildConfiguration section */\n";
            for (const auto configuration : configurations)
                configuration->output(file);
            file << "/* End XCBuildConfiguration section */\n";

            file << "\n/* Begin XCConfigurationList section */\n";
            for (const auto configurationList : configurationLists)
                configurationList->output(file);
            file << "/* End XCConfigurationList section */\n";

            file << "\t};\n"
            "\trootObject = " << pbxProject.getId() << " /* " << pbxProject.getName() << " */;\n"
            "}\n";
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
