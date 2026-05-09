#ifndef FLUXION_SCENE_SCENEMANAGER_HPP
#define FLUXION_SCENE_SCENEMANAGER_HPP

#include <memory>
#include <vector>

namespace fluxion::scene
{
    class Scene;

    class SceneManager final
    {
    public:
        SceneManager() = default;
        ~SceneManager();

        SceneManager(const SceneManager&) = delete;
        SceneManager& operator=(const SceneManager&) = delete;

        SceneManager(SceneManager&&) = delete;
        SceneManager& operator=(SceneManager&&) = delete;

        void draw();

        void setScene(Scene& scene);

        template <class T> void setScene(std::unique_ptr<T> scene)
        {
            setScene(*scene);
            ownedScenes.push_back(std::move(scene));
        }

        bool removeScene(const Scene& scene);

        auto getScene() const noexcept { return scenes.empty() ? nullptr : scenes.back(); }

        void calculateProjection();

    private:
        std::vector<Scene*> scenes;
        std::vector<std::unique_ptr<Scene>> ownedScenes;
    };
}

#endif // FLUXION_SCENE_SCENEMANAGER_HPP
