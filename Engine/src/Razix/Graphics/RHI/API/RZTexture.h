#pragma once

#include "Razix/Core/RZLog.h"

#include "Razix/Graphics/Resources/IRZResource.h"

#include "Razix/Graphics/RHI/API/Data/RZTextureData.h"
#include "Razix/Graphics/RHI/API/RZDescriptorSet.h"

#include "Razix/Graphics/RHI/API/RZAPIDesc.h"

namespace Razix {
    namespace Graphics {

        // Forward Declaration
        struct RZTextureDesc;

        /**
         * A class that manages Textures/Image resources for the Engine
         * @brief It manages creation and conversion of Image resources, also stores in a custom Engine Format depending on how it's being used
         */
        // TODO!!!: All will be under RZTexture and will use the TextureType struct to identify the type of the Texture
        // We will use a common set of functions under RZTexture to create various textures and of course the API ones can still be split
        // into different classes derived from RZTexture
        // TODO: Calculate size properly for manually set texture data
        // TODO: Add support and Utility functions for sRGB textures
        // TODO: Hide CreateXXX Functions and Replace all pointers with Handles!!!
        class RAZIX_API RZTexture : public IRZResource<RZTexture>
        {
        public:
            /* Default constructor, texture resource is done on demand */
            RZTexture() {}
            /* Virtual destructor enables the API implementation to delete it's resources */
            RAZIX_VIRTUAL_DESCTURCTOR(RZTexture)

            RAZIX_NONCOPYABLE_CLASS(RZTexture)

            GET_INSTANCE_SIZE;

            /**
             * Calculates the Mip Map count based on the Width and Height of the texture
             *
             * @param width     The width of the Texture
             * @param height    The height of the texture
             */
            static u32           calculateMipMapCount(u32 width, u32 height);
            static TextureFormat bitsToTextureFormat(u32 bits);

            /* Binds the Texture resource to the Pipeline */
            virtual void Bind(u32 slot) = 0;
            /* Unbinds the Texture resource from the pipeline */
            virtual void Unbind(u32 slot) = 0;

            /* Resize the texture */
            virtual void Resize(u32 width, u32 height) {}

            /* Gets the handle to the underlying API texture instance */
            virtual void* GetAPIHandlePtr() const = 0;

            virtual void SetData(const void* pixels) {}

            virtual int32_t ReadPixels(u32 x, u32 y) = 0;

            virtual void GenerateMips() {}

            virtual void UploadToBindlessSet() {}

            const RZTextureDesc& getDescription() const { return m_Desc; }

            /* Returns the name of the texture resource */
            std::string getName() const { return m_Desc.name; }
            /* returns the width of the texture */
            u32 getWidth() const { return m_Desc.width; }
            /* returns the height of the texture */
            u32 getHeight() const { return m_Desc.height; }
            /* Gets the size of the texture resource */
            uint64_t getSize() const { return m_Size; }
            /* Returns the type of the texture */
            TextureType getType() const { return m_Desc.type; }
            void        setType(TextureType type) { m_Desc.type = type; }
            /* Returns the internal format of the texture */
            TextureFormat getFormat() const { return m_Desc.format; }
            /* Returns the virtual path of the texture resource */
            std::string getPath() const { return m_VirtualPath; }
            Filtering   getFilterMode() { return m_Desc.filtering; }
            Wrapping    getWrapMode() { return m_Desc.wrapping; }

            /* Generates the descriptor set for the texture */
            void             generateDescriptorSet();
            RZDescriptorSet* getDescriptorSet() { return m_DescriptorSet; }

            u32  getCurrentMipLevel() { return m_CurrentMipRenderingLevel; }
            void setCurrentMipLevel(u32 idx) { m_CurrentMipRenderingLevel = idx; }

            u32  getCurrentArrayLayer() { return m_BaseArrayLayer; }
            void setCurrentArrayLayer(u32 idx) { m_BaseArrayLayer = idx; }

            u32 getLayersCount() { return m_Desc.layers; }
            u32 getMipsCount() { return m_TotalMipLevels; }
            // TODO: Add function to SetCurrentArrayAccessIdx kinda thing

            RAZIX_INLINE bool isRT() const { return m_IsRenderTexture; }

        protected:
            std::string      m_VirtualPath;                     /* The virtual path of the texture                             */
            uint64_t         m_Size;                            /* The size of the texture resource                            */
            RZDescriptorSet* m_DescriptorSet;                   /* Descriptor set for the image                                */
            RZTextureDesc    m_Desc;                            /* Texture properties and create desc                          */
            u32              m_TotalMipLevels           = 1;    /* Total Mips, Calculated by a formula except for RZCubeMap    */
            u32              m_CurrentMipRenderingLevel = 0;    /* Current mip level to which we are rendering to (as RT)      */
            u32              m_BaseArrayLayer           = 0;    /* Current face/array layer being accesses                     */
            bool             m_IsRenderTexture          = true; /* Any texture not imported from file and created is a RT      */

        private:
            static void Create(void* where, const RZTextureDesc& desc RZ_DEBUG_NAME_TAG_E_ARG);
            static void CreateFromFile(void* where, const RZTextureDesc& desc, const std::string& filePath RZ_DEBUG_NAME_TAG_E_ARG);

            friend class RZResourceManager;
        };
    }    // namespace Graphics
}    // namespace Razix