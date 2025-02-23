#pragma once

#include "Razix/Graphics/Materials/RZMaterialData.h"

namespace Razix {
    namespace Graphics {

        /* Not an interface yet, this is a hard coded PBR material as of this iteration of the engine */
        class RAZIX_API RZMaterial
        {
        public:
            /* Crates the material with the given shader and it's properties */
            RZMaterial(RZShaderHandle shader);
            virtual ~RZMaterial() {}

            void Destroy();

            /* Creates a 1x1 default pink 2D texture */
            static void InitDefaultTexture();
            /* Destroys the default texture created */
            static void            ReleaseDefaultTexture();
            static RZTextureHandle GetDefaultTexture() { return s_DefaultTexture; }
            /* Static Getter and setter for the material workflow */
            WorkFlow          getWorkflow() { return m_Workflow; }
            RAZIX_INLINE void setWorkflow(WorkFlow workflow) { m_Workflow = workflow; }

            /* Overrides the default material properties and textures by loading the material file and de-serializing it */
            void loadFromFile(const std::string& path);
            void saveToFile(const std::string& path = "");
            void loadMaterialTexturesFromFiles(MaterialTexturePaths paths);
            void createDescriptorSet();

            RAZIX_INLINE MaterialTextures& getTextures() { return m_MaterialTextures; }
            void                           setTextures(MaterialTextures& textures);

            RAZIX_INLINE const MaterialProperties& getProperties() const { return m_MaterialData.m_MaterialProperties; }
            void                                   setProperties(MaterialProperties& props);

            RAZIX_INLINE const MaterialTexturePaths& getTexturePaths() const { return m_MaterialData.m_MaterialTexturePaths; }
            void                                     setTexturePaths(MaterialTexturePaths& paths);

            void Bind(RZPipeline* pipeline = nullptr, RZCommandBuffer* cmdBuffer = nullptr);

            RAZIX_INLINE bool& getTexturesUpdated() { return m_TexturesUpdated; }
            RAZIX_INLINE void  setTexturesUpdated(bool isUpdated) { m_TexturesUpdated = isUpdated; }

            RAZIX_INLINE const std::string& getName() { return m_Name; }
            RAZIX_INLINE void               setName(const std::string& name) { m_Name = name; }

            RAZIX_INLINE RZDescriptorSet* getDescriptorSet() { return m_DescriptorSet; }

        private:
            //RZPipeline*                   m_Pipeline; // Diffifult to be own as Material can't have knowledge of the RTs in a pass and RZPipeline needs that before hand along with shader

            static RZTextureHandle s_DefaultTexture;
            MaterialData           m_MaterialData;
            MaterialTextures       m_MaterialTextures;
            std::string            m_Name                  = "PBR material";
            RZShaderHandle         m_Shader                = {};
            RZDescriptorSet*       m_DescriptorSet         = nullptr;
            RZUniformBufferHandle  m_MaterialPropertiesUBO = {};
            bool                   m_TexturesUpdated       = false;
            WorkFlow               m_Workflow              = WorkFlow::PBR_WORKFLOW_METALLIC_ROUGHTNESS;
        };

        static RZMaterial* DefaultMaterial = nullptr;

        RZMaterial* GetDefaultMaterial();

    }    // namespace Graphics
}    // namespace Razix
