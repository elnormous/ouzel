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

            void output(std::ostream& stream, size_t indent) const
            {
                auto filename = std::string(path);
                auto extension = path.getExtension();
                if (sourceTree == PbxSourceTree::BuildProductsDir)
                {
                    auto fileType = std::string("wrapper.application");
                    stream << std::string(indent, '\t') << getId() << " /* " << getName() << " */ = {"
                        "isa = " << getIsa() << "; "
                        "explicitFileType = " << fileType << "; "
                        "includeInIndex = " << 0 << "; "
                        "path = " << formatString(filename) << "; "
                        "sourceTree = " << formatString(sourceTreeToString(sourceTree)) << "; };\n";
                }
                else
                {
                    auto fileType = extension == "plist" ? "text.plist.xml" : "sourcecode.cpp." + extension;
                    stream << std::string(indent, '\t') << getId() << " /* " << getName() << " */ = {"
                        "isa = " << getIsa() << "; "
                        "lastKnownFileType = " << fileType << "; "
                        "path = " << formatString(filename) << "; "
                        "sourceTree = " << formatString(sourceTreeToString(sourceTree)) << "; };\n";
                }

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

            void output(std::ostream& stream, size_t indent) const
            {
                auto filename = std::string(fileRef.getPath());
                stream << std::string(indent, '\t') << getId() << " /* " << getName() << " */ = {isa = " << getIsa() << "; fileRef = " << formatReference(fileRef) << "; };\n";
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

            void output(std::ostream& stream, size_t indent) const
            {
                stream << std::string(indent, '\t') << getId() << " ";

                if (!getName().empty())
                    stream << "/* " << getName() << " */ ";

                stream << "= {\n" <<
                    std::string(indent + 1, '\t') << "isa = " << getIsa() << ";\n" <<
                    std::string(indent + 1, '\t') << "children = (\n";

                for (auto child : children)
                    stream << std::string(indent + 2, '\t') << formatReference(*child) << ",\n";

                stream << std::string(indent + 1, '\t') << ");\n";

                if (!std::string(path).empty())
                    stream << std::string(indent + 1, '\t') << "path = " << formatString(path) << ";\n";
                else if (!getName().empty())
                    stream << std::string(indent + 1, '\t') << "name = " << formatString(getName()) << ";\n";

                stream << std::string(indent + 1, '\t') << "sourceTree = " << formatString(sourceTreeToString(sourceTree)) << ";\n" <<
                    std::string(indent, '\t') << "};\n";
            }

        private:
            storage::Path path;
            std::vector<const PbxFileElement*> children;
            PbxSourceTree sourceTree;
        };

        class XcBuildConfiguration final: public Object
        {
        public:
            XcBuildConfiguration(const std::string& n):
                Object{"XCBuildConfiguration", n} {}

            void output(std::ostream& stream, size_t indent) const
            {
                stream << std::string(indent, '\t') << getId() << " /* " << getName() << " */ = {\n" <<
                    std::string(indent + 1, '\t') << "isa = " << getIsa() << ";\n" <<
                    std::string(indent + 1, '\t') << "buildSettings = {\n" <<
                    std::string(indent + 1, '\t') << "};\n" <<
                    std::string(indent + 1, '\t') << "name = " << formatString(getName()) << ";\n" <<
                    std::string(indent, '\t') << "};\n";
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

            void output(std::ostream& stream, size_t indent) const
            {
                stream << std::string(indent, '\t') << getId() << " /* " << getName() << " */ = {\n" <<
                    std::string(indent + 1, '\t') << "isa = " << getIsa() << ";\n" <<
                    std::string(indent + 1, '\t') << "buildConfigurations = (\n";

                for (auto configuration : configurations)
                    stream << std::string(indent + 2, '\t') << formatReference(*configuration) << ",\n";

                stream << std::string(indent + 1, '\t') << ");\n" <<
                    std::string(indent + 1, '\t') << "defaultConfigurationIsVisible = " << 0 << ";\n" <<
                    std::string(indent + 1, '\t') << "defaultConfigurationName = " << formatString(defaultConfiguration.getName()) << ";\n" <<
                    std::string(indent, '\t') << "};\n";
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

            void output(std::ostream& stream, size_t indent) const
            {
                stream << std::string(indent, '\t') << getId() << " /* " << getName() << " */ = {\n" <<
                    std::string(indent + 1, '\t') << "isa = " << getIsa() << ";\n" <<
                    std::string(indent + 1, '\t') << "buildActionMask = " << 2147483647 << ";\n" <<
                    std::string(indent + 1, '\t') << "files = (\n";
                for (auto file : files)
                    stream << std::string(indent + 2, '\t') << formatReference(*file) << ",\n";
                stream << std::string(indent + 1, '\t') << ");\n" <<
                    std::string(indent + 1, '\t') << "runOnlyForDeploymentPostprocessing = " << 0 << ";\n";
                stream << std::string(indent, '\t') << "};\n";
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

            void output(std::ostream& stream, size_t indent) const
            {
                stream << std::string(indent, '\t') << getId() << " /* " << getName() << " */ = {\n" <<
                    std::string(indent + 1, '\t') << "isa = " << getIsa() << ";\n" <<
                    std::string(indent + 1, '\t') << "buildConfigurationList = " << formatReference(buildConfigurationList) << ";\n" <<
                    std::string(indent + 1, '\t') << "buildPhases = (\n";
                for (auto buildPhase : buildPhases)
                    stream << std::string(indent + 2, '\t') << formatReference(*buildPhase) << ",\n";
                stream << std::string(indent + 1, '\t') << ");\n" <<
                    std::string(indent + 1, '\t') << "buildRules = (\n" <<
                    std::string(indent + 1, '\t') << ");\n" <<
                    std::string(indent + 1, '\t') << "dependencies = (\n" <<
                    std::string(indent + 1, '\t') << ");\n" <<
                    std::string(indent + 1, '\t') << "name = " << formatString(getName()) << ";\n" <<
                    std::string(indent + 1, '\t') << "productName = " << formatString(getName()) << ";\n" <<
                    std::string(indent + 1, '\t') << "productReference = " << formatReference(productReference) << ";\n" <<
                    std::string(indent + 1, '\t') << "productType = " << "\"com.apple.product-type.application\"" << ";\n";
                stream << std::string(indent, '\t') << "};\n";
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

            void output(std::ostream& stream, size_t indent) const
            {
                stream << std::string(indent, '\t') << getId() << " /* " << getName() << " */ = {\n" <<
                    std::string(indent + 1, '\t') << "isa = " << getIsa() << ";\n" <<
                    std::string(indent + 1, '\t') << "attributes = {\n" <<
                    std::string(indent + 2, '\t') << "LastUpgradeCheck = " << "0800" << ";\n" <<
                    std::string(indent + 2, '\t') << "ORGANIZATIONNAME = " << formatString(organization) << ";\n";
                stream << std::string(indent + 1, '\t') << "};\n" <<
                    std::string(indent + 1, '\t') << "buildConfigurationList = " << buildConfigurationList.getId() << " /* " << buildConfigurationList.getName() << " */;\n" <<
                    std::string(indent + 1, '\t') << "compatibilityVersion = " << "\"Xcode 9.3\"" << ";\n" <<
                    std::string(indent + 1, '\t') << "developmentRegion = " << "en" << ";\n" <<
                    std::string(indent + 1, '\t') << "hasScannedForEncodings = " << 0 << ";\n" <<
                    std::string(indent + 1, '\t') << "knownRegions = (\n" <<
                    std::string(indent + 2, '\t') << "en,\n" <<
                    std::string(indent + 2, '\t') << "Base,\n" <<
                    std::string(indent + 1, '\t') << ");\n" <<
                    std::string(indent + 1, '\t') << "mainGroup = " << formatReference(mainGroup) << ";\n" <<
                    std::string(indent + 1, '\t') << "productRefGroup = " << formatReference(productRefGroup) << ";\n" <<
                    std::string(indent + 1, '\t') << "projectDirPath = " << "\"\"" << ";\n" <<
                    std::string(indent + 1, '\t') << "projectRoot = " << "\"\"" << ";\n" <<
                    std::string(indent + 1, '\t') << "targets = (\n";

                for (auto target : targets)
                    stream << std::string(indent + 2, '\t') << formatReference(*target) << ",\n";

                stream << std::string(indent + 1, '\t') << ");\n" <<
                    std::string(indent, '\t') << "};\n";
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
                                                                   std::vector<const PbxBuildPhase*>{&buildSourcesPhase},
                                                                   productFile);
                nativeTargets.push_back(&nativeTarget);
                targets.push_back(&nativeTarget);
            }

            const auto& pbxProject = create<PbxProject>("Project object",
                                                        project.getOrganization(),
                                                        projectConfigurationList,
                                                        mainGroup,
                                                        productRefGroup,
                                                        targets);

            std::ofstream file(projectFile, std::ios::trunc);
            file << "// !$*UTF8*$!\n"
                "{\n" <<
                '\t' << "archiveVersion = 1;\n" <<
                '\t' << "classes = {\n" <<
                '\t' << "};\n" <<
                '\t' << "objectVersion = 46;\n" <<
                '\t' << "objects = {\n";

            // PBXBuildFile section
            for (const auto buildFile : buildFiles)
                buildFile->output(file, 2);

            // PBXFileReference section
            for (const auto fileReference : fileReferences)
                fileReference->output(file, 2);

            // frameworks buid phase
            // PBXFrameworksBuildPhase section
            // TODO

            // PBXGroup section
            for (const auto group : groups)
                group->output(file, 2);

            // PBXNativeTarget section
            for (const auto nativeTarget : nativeTargets)
                nativeTarget->output(file, 2);

            // PBXProject section
            pbxProject.output(file, 2);

            // resource build phases
            // PBXResourcesBuildPhase section
            // TODO

            // source file build phase
            // PBXSourcesBuildPhase section
            for (const auto sourcesBuildPhase : sourcesBuildPhases)
                sourcesBuildPhase->output(file, 2);

            // configurations
            // XCBuildConfiguration section
            for (const auto configuration : configurations)
                configuration->output(file, 2);

            // XCConfigurationList section
            for (const auto configurationList : configurationLists)
                configurationList->output(file, 2);

            file << '\t' << "};\n" <<
                '\t' << "rootObject = " << formatReference(pbxProject) << ";\n"
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
