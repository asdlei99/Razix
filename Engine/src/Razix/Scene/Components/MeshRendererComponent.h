#pragma once

#include "Razix/Graphics/Loaders/RZMeshLoader.h"
#include "Razix/Graphics/Materials/RZMaterial.h"
#include "Razix/Graphics/RZMesh.h"
#include "Razix/Graphics/RZMeshFactory.h"

#include <cereal/cereal.hpp>

namespace Razix {

    namespace Graphics {
        class RZMesh;
        enum MeshPrimitive : int;
        class RZMaterial;
    }    // namespace Graphics

    /**
     * Mesh renderer component references a mesh that will taken by the render to render a mesh on the 3D scene
     * It holds the reference to a 3D model or a primitive mesh to be rendered, so if a model is instantiated as
     * a Entity in the scene, each of it's children(which are also entities) will have a mesh renderer component
     * will be instantiated as entities in the scene with a Hierarchy, Transform,Tag, Active and default components
     * attached to them, these all meshes are taken at once by the Renderer and rendered to the scene, any additional
     * information required to rendered can be inferred as needed
     */
    struct RAZIX_API MeshRendererComponent
    {
        Graphics::RZMesh*       Mesh;
        Graphics::MeshPrimitive primitive;

        // TODO: Enable these
        bool enableBoundingBoxes;
        bool receiveShadows;

        MeshRendererComponent();
        MeshRendererComponent(const std::string& filePath);
        MeshRendererComponent(Graphics::MeshPrimitive primitive);
        MeshRendererComponent(Graphics::RZMesh* mesh);
        MeshRendererComponent(const MeshRendererComponent&) = default;

        template<class Archive>
        void load(Archive& archive)
        {
            int prim = -1;
            archive(cereal::make_nvp("Primitive", prim));
            primitive = Graphics::MeshPrimitive(prim);

            if (prim >= 0)
                Mesh = Graphics::MeshFactory::CreatePrimitive(primitive);
            std::string meshName;
            archive(cereal::make_nvp("MeshName", meshName));
            std::string meshPath;
            archive(cereal::make_nvp("MeshPath", meshPath));

            if (!Mesh || !meshPath.empty())
                Mesh = Razix::Graphics::loadMesh(meshPath);

            if (Mesh) {
                Mesh->setName(meshName);
                Mesh->setPath(meshPath);
            }

            // Load/Create a new Material (override the save location)
            std::string materialPath;
            archive(cereal::make_nvp("MaterialPath", materialPath));
            if (!materialPath.empty()) {
                // Since we have the path to a material file load it, deserialize it and create the material
                Mesh->getMaterial()->loadFromFile(materialPath);
            }
        }

        template<class Archive>
        void save(Archive& archive) const
        {
            archive(cereal::make_nvp("Primitive", primitive));
            if (Mesh) {
                archive(cereal::make_nvp("MeshName", Mesh->getName()));
                archive(cereal::make_nvp("MeshPath", Mesh->getPath()));

                auto matPath = "//Assets/Materials/" + Mesh->getMaterial()->getName() + ".rzmaterial";
                archive(cereal::make_nvp("MaterialPath", matPath));
                Mesh->getMaterial()->saveToFile();
            }
        }
    };
}    // namespace Razix