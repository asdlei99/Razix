#pragma once

#include "Razix/Graphics/RHI/API/RZDescriptorSet.h"

#ifdef RAZIX_RENDER_API_VULKAN

    #include <vulkan/vulkan.h>

namespace Razix {
    namespace Graphics {

    #define MAX_BUFFER_INFOS      32
    #define MAX_IMAGE_INFOS       32
    #define MAX_WRITE_DESCTIPTORS 32

        /* Vulkan implementation of the descriptor sets, these are used to bind/map the uniform, storage and image resources to shaders */
        class VKDescriptorSet : public RZDescriptorSet
        {
        public:
            VKDescriptorSet(const std::vector<RZDescriptor>& descriptors, bool layoutTransition RZ_DEBUG_NAME_TAG_E_ARG);

            void UpdateSet(const std::vector<RZDescriptor>& descriptors, bool layoutTransition = true) override;
            void Destroy() override;

            inline const VkDescriptorSet& getDescriptorSet() const { return m_DescriptorSet; }

        private:
            VkDescriptorPool        m_DescriptorPool; /* The descriptor pool from which the descriptor sets will be allocated from                                */
            VkDescriptorSet         m_DescriptorSet;  /* Each frame gets a descriptor set that helps in mapping the necessary data to the shader slots            */
            VkDescriptorBufferInfo* m_BufferInfoPool         = nullptr;
            VkDescriptorImageInfo*  m_ImageInfoPool          = nullptr;
            VkWriteDescriptorSet*   m_WriteDescriptorSetPool = nullptr;
            VkDescriptorSetLayout   setLayout;
        };

    }    // namespace Graphics
}    // namespace Razix
#endif