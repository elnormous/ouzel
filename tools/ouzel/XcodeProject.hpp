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
            Object(const std::string& n): name{n} {}
            virtual ~Object() = default;

            const std::string& getId() const noexcept { return id; }
            const std::string& getName() const noexcept { return name; }

        private:
            std::string id = generateId();
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
            Object{std::string{p}}, path{p}, type{t} {}
            const storage::Path& getPath() const noexcept { return path; }

            void output(std::ofstream& file) const
            {
                auto filename = std::string(path);
                auto extension = path.getExtension();
                if (type == Type::Build)
                {
                    auto fileType = extension == "plist" ? "text.plist.xml" : "sourcecode.cpp." + extension;
                    file << "\t\t" << getId() << " /* " << filename << " */ = {isa = PBXFileReference; lastKnownFileType = " << fileType << "; path = " << formatString(filename) << "; sourceTree = " << formatString("<group>") << "; };\n";
                }
                else if (type == Type::Product)
                    file << "\t\t" << getId() << " /* " << filename << " */ = {isa = PBXFileReference; explicitFileType = wrapper.application; includeInIndex = 0; path = " << formatString(filename) << "; sourceTree = BUILT_PRODUCTS_DIR; };\n";
            }

        private:
            storage::Path path;
            Type type = Type::Build;
        };

        class PbxBuildFile final: public Object
        {
        public:
            PbxBuildFile(const PbxFileReference& ref):
                Object{ref.getName()}, fileRef{ref} {}

            void output(std::ofstream& file) const
            {
                auto filename = std::string(fileRef.getPath());
                file << "\t\t" << getId() << " /* " << filename << " in Sources */ = {isa = PBXBuildFile; fileRef = " << formatReference(fileRef) << "; };\n";
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
            Object{n}, path{p}, children{c} {}

            void output(std::ofstream& file) const
            {
                file << "\t\t" << getId() << " ";

                if (!getName().empty())
                    file << "/* " << getName() << " */ ";

                file << "= {\n"
                "\t\t\tisa = PBXGroup;\n"
                "\t\t\tchildren = (\n";

                for (auto child : children)
                    file << "\t\t\t\t" << formatReference(*child) << ",\n";

                file << "\t\t\t);\n";

                if (!std::string(path).empty())
                    file << "\t\t\tpath = " << formatString(path) << ";\n";
                else if (!getName().empty())
                    file << "\t\t\tname = " << formatString(getName()) << ";\n";

                file << "\t\t\tsourceTree = " << formatString("<group>") << ";\n"
                "\t\t};\n";
            }

        private:
            storage::Path path;
            std::vector<const Object*> children;
        };

        class XcBuildConfiguration final: public Object
        {
        public:
            XcBuildConfiguration(const std::string& n): Object{n} {}

            void output(std::ofstream& file) const
            {
                file << "\t\t" << getId() << " /* " << getName() << " */ = {\n"
                "\t\t\tisa = XCBuildConfiguration;\n"
                "\t\t\tbuildSettings = {\n"
                "\t\t\t};\n"
                "\t\t\tname = " << formatString(getName()) << ";\n"
                "\t\t};\n";
            }

        private:
        };

        class XcBuildConfigurationList final: public Object
        {
        public:
            XcBuildConfigurationList(const std::string& n,
                                     const std::vector<const XcBuildConfiguration*>& c,
                                     const XcBuildConfiguration& defaultConfig):
                Object{n},
                configurations{c},
                defaultConfiguration{defaultConfig} {}

            void output(std::ofstream& file) const
            {
                file << "\t\t" << getId() << " /* " << getName() << " */ = {\n"
                "\t\t\tisa = XCConfigurationList;\n"
                "\t\t\tbuildConfigurations = {\n";

                for (auto configuration : configurations)
                    file << "\t\t\t\t" << formatReference(*configuration) << ",\n";

                file << "\t\t\t};\n"
                "\t\t\tdefaultConfigurationIsVisible = 0;\n"
                "\t\t\tdefaultConfigurationName = " << formatString(defaultConfiguration.getName()) << ";\n"
                "\t\t};\n";
            }

        private:
            std::vector<const XcBuildConfiguration*> configurations;
            const XcBuildConfiguration& defaultConfiguration;
        };

        class PbxNativeTarget final: public Object
        {
        public:
            PbxNativeTarget(const std::string& n,
                            const XcBuildConfigurationList& buildConfigList,
                            const PbxFileReference& product):
                Object{n},
                buildConfigurationList{buildConfigList},
                productReference{product} {}

            void output(std::ofstream& file) const
            {
                file << "\t\t" << getId() << " /* " << getName() << " */ = {\n"
                "\t\t\tisa = PBXNativeTarget;\n"
                "\t\t\tbuildConfigurationList = " << formatReference(buildConfigurationList) << ";\n"
                "\t\t\tbuildPhases = (\n";
                file << "\t\t\t);\n"
                "\t\t\tbuildRules = (\n"
                "\t\t\t);\n"
                "\t\t\tdependencies = (\n"
                "\t\t\t);\n"
                "\t\t\tname = " << formatString(getName()) << ";\n"
                "\t\t\tproductName = " << formatString(getName()) << ";\n"
                "\t\t\tproductReference = " << formatReference(productReference) << ";\n"
                "\t\t\tproductType = \"com.apple.product-type.application\";\n";
                file << "\t\t};\n";
            }

        private:
            const XcBuildConfigurationList& buildConfigurationList;
            const PbxFileReference& productReference;
        };

        class PbxProject final: public Object
        {
        public:
            PbxProject(const std::string& n,
                       const std::string& org,
                       const XcBuildConfigurationList& buildConfigList,
                       const PbxGroup& mainGrp,
                       const PbxGroup& productRefGrp,
                       const std::vector<const PbxNativeTarget*>& t):
                Object{n},
                organization{org},
                buildConfigurationList{buildConfigList},
                mainGroup(mainGrp),
                productRefGroup(productRefGrp),
                targets{t} {}

            void output(std::ofstream& file) const
            {
                file << "\t\t" << getId() << " /* " << getName() << " */ = {\n"
                "\t\t\tisa = PBXProject;\n"
                "\t\t\tattributes = {\n"
                "\t\t\t\tLastUpgradeCheck = 0800;\n"
                "\t\t\t\tORGANIZATIONNAME = " << formatString(organization) << ";\n";
                file << "\t\t\t};\n"
                "\t\t\tbuildConfigurationList = " << buildConfigurationList.getId() << " /* " << buildConfigurationList.getName() << " */;\n"
                "\t\t\tcompatibilityVersion = \"Xcode 9.3\";\n"
                "\t\t\tdevelopmentRegion = en;\n"
                "\t\t\thasScannedForEncodings = 0;\n"
                "\t\t\tknownRegions = (\n"
                "\t\t\t\ten,\n"
                "\t\t\t\tBase,\n"
                "\t\t\t);\n"
                "\t\t\tmainGroup = " << formatReference(mainGroup) << ";\n"
                "\t\t\tproductRefGroup = " << formatReference(productRefGroup) << ";\n"
                "\t\t\tprojectDirPath = \"\";\n"
                "\t\t\tprojectRoot = \"\";\n"
                "\t\t\ttargets = (\n";

                for (auto target : targets)
                    file << "\t\t\t\t" << formatReference(*target) << ",\n";

                file << "\t\t\t);\n"
                "\t\t};\n";
            }

        private:
            std::string organization;
            const XcBuildConfigurationList& buildConfigurationList;
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

            std::vector<const XcBuildConfigurationList*> configurationLists;
            const auto& projectConfigurationList = create<XcBuildConfigurationList>("Project",
                                                                                    std::vector<const XcBuildConfiguration*>{&debugConfiguration, &releaseConfiguration},
                                                                                    releaseConfiguration);
            configurationLists.push_back(&projectConfigurationList);

            std::vector<const PbxNativeTarget*> nativeTargets;
            // TODO: for each architecture
            {
                const auto& targetDebugConfiguration = create<XcBuildConfiguration>("Debug");
                configurations.push_back(&targetDebugConfiguration);

                const auto& targetReleaseConfiguration = create<XcBuildConfiguration>("Release");
                configurations.push_back(&targetReleaseConfiguration);

                const auto& targetConfigurationList = create<XcBuildConfigurationList>("Target",
                                                                                       std::vector<const XcBuildConfiguration*>{&targetDebugConfiguration, &targetReleaseConfiguration},
                                                                                       targetReleaseConfiguration);
                configurationLists.push_back(&targetConfigurationList);

                const auto& nativeTarget = create<PbxNativeTarget>(project.getName(), targetConfigurationList, productFile);
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
