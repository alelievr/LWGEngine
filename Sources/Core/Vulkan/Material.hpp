#pragma once

#include <iostream>
#include <string>

#include "Core/Texture.hpp"
#include "Core/Vulkan/UniformBuffer.hpp"
#include "IncludeDeps.hpp"

#include VULKAN_INCLUDE
#include GLM_INCLUDE
#include "VulkanInstance.hpp"
#include "Vk.hpp"
#include "Core/Shaders/ShaderProgram.hpp"

#define PER_MATERIAL_BINDING_INDEX		3
#define TRILINEAR_CLAMP_BINDING_INDEX	1

namespace LWGC
{
	enum class TextureBinding
	{
		Albedo = 2,
		Normal = 3,
		Height = 4,
		Smoothness = 5,
	};

	class SwapChain;
	class RenderPass;

	class		Material
	{
		private:
			struct LWGC_PerMaterial
			{
				glm::vec4		albedo;
			};

			VkDescriptorSet							_descriptorSet;
			VkPipelineLayout						_pipelineLayout;
			VkPipeline								_pipeline;
			LWGC_PerMaterial						_perMaterial;
			UniformBuffer							_uniformPerMaterial;
			std::vector< VkSampler >				_samplers;
			VulkanInstance *						_instance;
			VkDevice								_device;
			SwapChain *								_swapChain;
			RenderPass *							_renderPass;
			ShaderProgram *							_program;
			std::vector< VkDescriptorSetLayout >	_setLayouts;
	
			void		CreateTextureSampler(void);
			void		CreateUniformBuffer(void);
			void		CreateDescriptorSets(void);
			void		CreatePipelineLayout(void);
			void		CompileShaders(void);
	
		public:
			Material(void);
			Material(const Material &);
			Material(const std::string & shader, VkShaderStageFlagBits stage);
			Material(const std::string & fragmentShader);
			Material(ShaderProgram * program);
			virtual ~Material(void);
	
			Material &	operator=(Material const & src);
	
			void	Initialize(SwapChain * swapchain, RenderPass * renderPass);
			void	CleanupPipeline(void) noexcept;
			void	CreatePipeline(void);
			void	UpdateUniformBuffer(void);

			VkPipeline			GetPipeline(void) const;
			VkPipelineLayout	GetPipelineLayout(void) const;
			uint32_t			GetDescriptorSetBinding(const std::string & setName) const;
			VkDescriptorSet		GetDescriptorSet(void) const;

			void				SetBuffer(uint32_t setIndex, uint32_t bindingIndex, VkBuffer buffer, size_t size);
			void				SetTexture(uint32_t setIndex, uint32_t bindingIndex, const Texture & texture, VkImageLayout imageLayout, VkDescriptorType descriptorType);
			void				SetTexture(TextureBinding binding, const Texture & texture, VkImageLayout imageLayout, VkDescriptorType descriptorType);
	};
	
	std::ostream &	operator<<(std::ostream & o, Material const & r);
}