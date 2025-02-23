// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZPBRLightingPass.h"

#include "Razix/Core/RZApplication.h"
#include "Razix/Core/RZMarkers.h"

#include "Razix/Graphics/RHI/API/RZCommandBuffer.h"
#include "Razix/Graphics/RHI/API/RZGraphicsContext.h"
#include "Razix/Graphics/RHI/API/RZIndexBuffer.h"
#include "Razix/Graphics/RHI/API/RZPipeline.h"
#include "Razix/Graphics/RHI/API/RZTexture.h"
#include "Razix/Graphics/RHI/API/RZUniformBuffer.h"
#include "Razix/Graphics/RHI/API/RZVertexBuffer.h"

#include "Razix/Graphics/RHI/RHI.h"

#include "Razix/Graphics/RZMesh.h"
#include "Razix/Graphics/RZMeshFactory.h"

#include "Razix/Graphics/RZShaderLibrary.h"

#include "Razix/Graphics/Materials/RZMaterial.h"

#include "Razix/Graphics/Passes/Data/BRDFData.h"
#include "Razix/Graphics/Passes/Data/FrameBlockData.h"
#include "Razix/Graphics/Passes/Data/GBufferData.h"
#include "Razix/Graphics/Passes/Data/GlobalData.h"

#include "Razix/Graphics/Resources/RZFrameGraphBuffer.h"

#include "Razix/Graphics/Resources/RZFrameGraphTexture.h"

#include "Razix/Scene/Components/RZComponents.h"

#include "Razix/Scene/RZScene.h"

namespace Razix {
    namespace Graphics {

        void RZPBRLightingPass::addPass(FrameGraph::RZFrameGraph& framegraph, FrameGraph::RZBlackboard& blackboard, Razix::RZScene* scene, RZRendererSettings& settings)
        {
            auto pbrShader = RZShaderLibrary::Get().getBuiltInShader(ShaderBuiltin::PBRIBL);

            Graphics::RZPipelineDesc pipelineInfo{};
            pipelineInfo.name                   = "PBR Lighting Pipeline";
            pipelineInfo.cullMode               = Graphics::CullMode::Front;
            pipelineInfo.depthBiasEnabled       = false;
            pipelineInfo.drawType               = Graphics::DrawType::Triangle;
            pipelineInfo.shader                 = pbrShader;
            pipelineInfo.transparencyEnabled    = true;
            pipelineInfo.colorAttachmentFormats = {Graphics::TextureFormat::RGBA32F};
            pipelineInfo.depthFormat            = Graphics::TextureFormat::DEPTH32F;
            pipelineInfo.depthTestEnabled       = true;
            pipelineInfo.depthWriteEnabled      = true;
            m_Pipeline                          = RZResourceManager::Get().createPipeline(pipelineInfo);

            auto& frameDataBlock       = blackboard.get<FrameData>();
            auto& sceneLightsDataBlock = blackboard.get<SceneLightsData>();
            auto& shadowData           = blackboard.get<SimpleShadowPassData>();
            auto& globalLightProbes    = blackboard.get<GlobalLightProbeData>();
            auto& brdfData             = blackboard.get<BRDFData>();
            //auto& gbufferData          = blackboard.get<GBufferData>();

            m_ScreenQuadMesh = Graphics::MeshFactory::CreatePrimitive(Razix::Graphics::MeshPrimitive::ScreenQuad);

            struct PBRPassBindingData
            {
                u32 shadowIdx;
                u32 irradIdx;
                u32 prefiltIdx;
                u32 brdfIdx;
            };

            //m_PBRPassBindingUBO = RZUniformBuffer::Create(sizeof(PBRPassBindingData), nullptr RZ_DEBUG_NAME_TAG_STR_E_ARG("PBRPassTextures UBO"));

            blackboard.add<SceneData>() = framegraph.addCallbackPass<SceneData>(
                "Pass.Builtin.Code.PBRLighting",
                [&](SceneData& data, FrameGraph::RZPassResourceBuilder& builder) {
                    builder.setAsStandAlonePass();

                    RZTextureDesc textureDesc{
                        .name   = "SceneHDR",
                        .width  = ResolutionToExtentsMap[Resolution::k1440p].x,
                        .height = ResolutionToExtentsMap[Resolution::k1440p].y,
                        .type   = TextureType::Texture_2D,
                        .format = TextureFormat::RGBA32F};

                    data.outputHDR = builder.create<FrameGraph::RZFrameGraphTexture>(textureDesc.name, CAST_TO_FG_TEX_DESC textureDesc);

                    textureDesc.name       = "SceneDepth";
                    textureDesc.format     = TextureFormat::DEPTH32F;
                    textureDesc.filtering  = {Filtering::Mode::NEAREST, Filtering::Mode::NEAREST},
                    textureDesc.type       = TextureType::Texture_Depth;
                    textureDesc.enableMips = false;

                    data.depth = builder.create<FrameGraph::RZFrameGraphTexture>(textureDesc.name, CAST_TO_FG_TEX_DESC textureDesc);

                    data.outputHDR = builder.write(data.outputHDR);
                    data.depth     = builder.write(data.depth);

                    builder.read(frameDataBlock.frameData);
                    builder.read(sceneLightsDataBlock.lightsDataBuffer);
                    builder.read(shadowData.shadowMap);
                    builder.read(shadowData.lightVP);
                    builder.read(globalLightProbes.environmentMap);
                    builder.read(globalLightProbes.diffuseIrradianceMap);
                    builder.read(globalLightProbes.specularPreFilteredMap);
                    builder.read(brdfData.lut);
                    //builder.read(gbufferData.Albedo_PosY);
                    //builder.read(gbufferData.Emissive_PosZ);
                    //builder.read(gbufferData.Normal_PosX);
                    //builder.read(gbufferData.MetRougAOAlpha);
                    //builder.read(gbufferData.Depth);
                },
                [=](const SceneData& data, FrameGraph::RZPassResourceDirectory& resources) {
                    RAZIX_PROFILE_FUNCTIONC(RZ_PROFILE_COLOR_GRAPHICS);

                    RAZIX_MARK_BEGIN("Pass.Builtin.PBRLighting", glm::vec4(1.0f, 0.5f, 0.0f, 1.0f));

                    RenderingInfo info{};
                    info.resolution       = Resolution::kCustom;
                    info.colorAttachments = {{resources.get<FrameGraph::RZFrameGraphTexture>(data.outputHDR).getHandle(), {true, ClearColorPresets::TransparentBlack}}};
                    info.depthAttachment  = {resources.get<FrameGraph::RZFrameGraphTexture>(data.depth).getHandle(), {true, ClearColorPresets::DepthOneToZero}};
                    info.extent           = {RZApplication::Get().getWindow()->getWidth(), RZApplication::Get().getWindow()->getHeight()};
                    info.resize           = true;

                    RHI::BeginRendering(RHI::GetCurrentCommandBuffer(), info);

                    // Set the Descriptor Set once rendering starts
                    static bool updatedSets = false;
                    if (!updatedSets) {
                        //RZDescriptor descriptor{};
                        //descriptor.bindingInfo.location.binding = 0;
                        //descriptor.bindingInfo.type    = DescriptorType::UNIFORM_BUFFER;
                        //descriptor.bindingInfo.stage   = ShaderStage::PIXEL;
                        //descriptor.uniformBuffer       = resources.get<FrameGraph::RZFrameGraphBuffer>(shadowData.lightVP).getHandle();

                        //RZDescriptor texdescriptor{};
                        //texdescriptor.bindingInfo.location.binding = 1;
                        //texdescriptor.bindingInfo.type    = DescriptorType::UNIFORM_BUFFER;
                        //texdescriptor.bindingInfo.stage   = ShaderStage::PIXEL;
                        //texdescriptor.uniformBuffer       = m_PBRPassBindingUBO;

                        //m_PBRBindingSet = RZDescriptorSet::Create({descriptor, texdescriptor} RZ_DEBUG_NAME_TAG_STR_E_ARG("PBR data Bindings Set"));

                        auto shadowMap = resources.get<FrameGraph::RZFrameGraphTexture>(shadowData.shadowMap).getHandle();

                        RZDescriptor csm_descriptor{};
                        csm_descriptor.bindingInfo.location.binding = 0;
                        csm_descriptor.bindingInfo.type             = DescriptorType::IMAGE_SAMPLER;
                        csm_descriptor.bindingInfo.stage            = ShaderStage::PIXEL;
                        csm_descriptor.texture                      = shadowMap;

                        RZDescriptor shadow_data_descriptor{};
                        shadow_data_descriptor.size                         = sizeof(SimpleShadowPassData);
                        shadow_data_descriptor.bindingInfo.location.binding = 1;
                        shadow_data_descriptor.bindingInfo.type             = DescriptorType::UNIFORM_BUFFER;
                        shadow_data_descriptor.bindingInfo.stage            = ShaderStage::PIXEL;
                        shadow_data_descriptor.uniformBuffer                = resources.get<FrameGraph::RZFrameGraphBuffer>(shadowData.lightVP).getHandle();

                        m_ShadowDataSet = RZDescriptorSet::Create({csm_descriptor, shadow_data_descriptor} RZ_DEBUG_NAME_TAG_STR_E_ARG("Shadow pass Bindings"));

                        auto irradianceMap  = resources.get<FrameGraph::RZFrameGraphTexture>(globalLightProbes.diffuseIrradianceMap).getHandle();
                        auto prefilteredMap = resources.get<FrameGraph::RZFrameGraphTexture>(globalLightProbes.specularPreFilteredMap).getHandle();
                        auto brdfLUT        = resources.get<FrameGraph::RZFrameGraphTexture>(brdfData.lut).getHandle();

                        // TODO: Enable this only if we use a skybox
                        RZDescriptor irradianceMap_descriptor{};
                        irradianceMap_descriptor.bindingInfo.location.binding = 0;
                        irradianceMap_descriptor.bindingInfo.type             = DescriptorType::IMAGE_SAMPLER;
                        irradianceMap_descriptor.bindingInfo.stage            = ShaderStage::PIXEL;
                        irradianceMap_descriptor.texture                      = irradianceMap;

                        RZDescriptor prefiltered_descriptor{};
                        prefiltered_descriptor.bindingInfo.location.binding = 1;
                        prefiltered_descriptor.bindingInfo.type             = DescriptorType::IMAGE_SAMPLER;
                        prefiltered_descriptor.bindingInfo.stage            = ShaderStage::PIXEL;
                        prefiltered_descriptor.texture                      = prefilteredMap;

                        RZDescriptor brdflut_descriptor{};
                        brdflut_descriptor.bindingInfo.location.binding = 2;
                        brdflut_descriptor.bindingInfo.type             = DescriptorType::IMAGE_SAMPLER;
                        brdflut_descriptor.bindingInfo.stage            = ShaderStage::PIXEL;
                        brdflut_descriptor.texture                      = brdfLUT;

                        m_PBRDataSet = RZDescriptorSet::Create({irradianceMap_descriptor, prefiltered_descriptor, brdflut_descriptor} RZ_DEBUG_NAME_TAG_STR_E_ARG("PBR data Bindings"));
#if 0

                        RZDescriptor gbuffer0_descriptor{};
                        gbuffer0_descriptor.bindingInfo.location.binding = 0;
                        gbuffer0_descriptor.bindingInfo.type    = DescriptorType::IMAGE_SAMPLER;
                        gbuffer0_descriptor.bindingInfo.stage   = ShaderStage::PIXEL;
                        gbuffer0_descriptor.texture             = resources.get<FrameGraph::RZFrameGraphTexture>(gbufferData.Normal_PosX).getHandle();

                        RZDescriptor gbuffer1_descriptor{};
                        gbuffer1_descriptor.bindingInfo.location.binding = 1;
                        gbuffer1_descriptor.bindingInfo.type    = DescriptorType::IMAGE_SAMPLER;
                        gbuffer1_descriptor.bindingInfo.stage   = ShaderStage::PIXEL;
                        gbuffer1_descriptor.texture             = resources.get<FrameGraph::RZFrameGraphTexture>(gbufferData.Albedo_PosY).getHandle();

                        RZDescriptor gbuffer2_descriptor{};
                        gbuffer2_descriptor.bindingInfo.location.binding = 2;
                        gbuffer2_descriptor.bindingInfo.type    = DescriptorType::IMAGE_SAMPLER;
                        gbuffer2_descriptor.bindingInfo.stage   = ShaderStage::PIXEL;
                        gbuffer2_descriptor.texture             = resources.get<FrameGraph::RZFrameGraphTexture>(gbufferData.Emissive_PosZ).getHandle();

                        RZDescriptor gbuffer3_descriptor{};
                        gbuffer3_descriptor.bindingInfo.location.binding = 3;
                        gbuffer3_descriptor.bindingInfo.type    = DescriptorType::IMAGE_SAMPLER;
                        gbuffer3_descriptor.bindingInfo.stage   = ShaderStage::PIXEL;
                        gbuffer3_descriptor.texture             = resources.get<FrameGraph::RZFrameGraphTexture>(gbufferData.MetRougAOAlpha).getHandle();

                        m_GBufferDataSet = RZDescriptorSet::Create({gbuffer0_descriptor, gbuffer1_descriptor, gbuffer2_descriptor, gbuffer3_descriptor} RZ_DEBUG_NAME_TAG_STR_E_ARG("GBuffer Bindings"));

#endif
                        updatedSets = true;
                    }

                    auto shadowMap      = resources.get<FrameGraph::RZFrameGraphTexture>(shadowData.shadowMap).getHandle();
                    auto irradianceMap  = resources.get<FrameGraph::RZFrameGraphTexture>(globalLightProbes.diffuseIrradianceMap).getHandle();
                    auto prefilteredMap = resources.get<FrameGraph::RZFrameGraphTexture>(globalLightProbes.specularPreFilteredMap).getHandle();
                    auto brdfLUT        = resources.get<FrameGraph::RZFrameGraphTexture>(brdfData.lut).getHandle();

                    //PBRPassBindingData bindingData{};
                    //bindingData.shadowIdx  = shadowMap.getIndex();
                    //bindingData.irradIdx   = irradianceMap.getIndex();
                    //bindingData.prefiltIdx = prefilteredMap.getIndex();
                    //bindingData.brdfIdx    = brdfLUT.getIndex();

                    //m_PBRPassBindingUBO->SetData(sizeof(PBRPassBindingData), &bindingData);

                    RHI::BindPipeline(m_Pipeline, RHI::GetCurrentCommandBuffer());

                    scene->drawScene(m_Pipeline, {.userSets = {m_ShadowDataSet, m_PBRDataSet}});

                    RHI::EndRendering(RHI::GetCurrentCommandBuffer());
                    RAZIX_MARK_END();
                });
        }

        void RZPBRLightingPass::destroy()
        {
            RZResourceManager::Get().destroyPipeline(m_Pipeline);
            //m_PBRBindingSet->Destroy();
            //m_PBRPassBindingUBO->Destroy();
        }
    }    // namespace Graphics
}    // namespace Razix