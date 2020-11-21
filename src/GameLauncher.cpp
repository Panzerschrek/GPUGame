#include "GameLauncher.hpp"
#include "Assert.hpp"


namespace GPUGame
{

namespace
{

namespace Shaders
{

#include "shaders/test.comp.h"

}

// Layout of this struct must match layout of same struct in shaders!
struct Uniforms
{
	float time_s= 0.0f;
};

} // namespace

GameLauncher::GameLauncher(WindowVulkan& window_vulkan)
	: vk_device_(window_vulkan.GetVulkanDevice())
	, viewport_size_(window_vulkan.GetViewportSize())
{
	const auto memory_properties= window_vulkan.GetMemoryProperties();

	{ // Prepare game framebuffer buffer.
		const vk::DeviceSize buffer_size= viewport_size_.width * viewport_size_.height * 4;

		vk_game_framebuffer_buffer_=
			vk_device_.createBufferUnique(
				vk::BufferCreateInfo(
					vk::BufferCreateFlags(),
					buffer_size,
					vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eTransferSrc));

		const vk::MemoryRequirements buffer_memory_requirements= vk_device_.getBufferMemoryRequirements(*vk_game_framebuffer_buffer_);

		vk::MemoryAllocateInfo vk_memory_allocate_info(buffer_memory_requirements.size);
		for(uint32_t i= 0u; i < memory_properties.memoryTypeCount; ++i)
		{
			if((buffer_memory_requirements.memoryTypeBits & (1u << i)) != 0 &&
				(memory_properties.memoryTypes[i].propertyFlags & vk::MemoryPropertyFlagBits::eDeviceLocal) != vk::MemoryPropertyFlags())
				vk_memory_allocate_info.memoryTypeIndex= i;
		}

		vk_game_framebuffer_buffer_memory_= vk_device_.allocateMemoryUnique(vk_memory_allocate_info);
		vk_device_.bindBufferMemory(*vk_game_framebuffer_buffer_, *vk_game_framebuffer_buffer_memory_, 0u);
	}

	{ // Create shader.
		game_shader_= vk_device_.createShaderModuleUnique(
			vk::ShaderModuleCreateInfo(
				vk::ShaderModuleCreateFlags(),
				sizeof(Shaders::test_comp),
				Shaders::test_comp));
	}

	{ // Create pipeline.

		const vk::DescriptorSetLayoutBinding descriptor_set_layout_bindings[1]
		{
			{
				0u,
				vk::DescriptorType::eStorageBuffer,
				1u,
				vk::ShaderStageFlagBits::eCompute,
				nullptr,
			},
		};
		vk_descriptor_set_layout_= vk_device_.createDescriptorSetLayoutUnique(
			vk::DescriptorSetLayoutCreateInfo(
				vk::DescriptorSetLayoutCreateFlags(),
				uint32_t(std::size(descriptor_set_layout_bindings)), descriptor_set_layout_bindings));

		const vk::PushConstantRange vk_push_constant_range(
			vk::ShaderStageFlagBits::eCompute,
			0u,
			sizeof(Uniforms));

		vk_pipeline_layout_= vk_device_.createPipelineLayoutUnique(
			vk::PipelineLayoutCreateInfo(
				vk::PipelineLayoutCreateFlags(),
				1u, &*vk_descriptor_set_layout_,
				1u, &vk_push_constant_range));

		vk_pipeline_= vk_device_.createComputePipelineUnique(
			nullptr,
			vk::ComputePipelineCreateInfo(
				vk::PipelineCreateFlags(),
				vk::PipelineShaderStageCreateInfo(
					vk::PipelineShaderStageCreateFlags(),
					vk::ShaderStageFlagBits::eCompute,
					*game_shader_,
					"main"),
				*vk_pipeline_layout_));
	}
	{ // create descriptor set pool
		const vk::DescriptorPoolSize vk_descriptor_pool_sizes[]
		{
			{
				vk::DescriptorType::eStorageBuffer,
				1u,
			},
		};

		vk_descriptor_pool_=
			vk_device_.createDescriptorPoolUnique(
				vk::DescriptorPoolCreateInfo(
					vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
					1u, // max sets.
					uint32_t(std::size(vk_descriptor_pool_sizes)), vk_descriptor_pool_sizes));
	}
	{ // Create descriptor set
		vk_descriptor_set_=
			std::move(
			vk_device_.allocateDescriptorSetsUnique(
				vk::DescriptorSetAllocateInfo(
					*vk_descriptor_pool_,
					1u, &*vk_descriptor_set_layout_)).front());

		const vk::DescriptorBufferInfo descriptor_buffer_info(
			*vk_game_framebuffer_buffer_,
			0u,
			viewport_size_.width * viewport_size_.height * 4);

		vk_device_.updateDescriptorSets(
			{
				{
					*vk_descriptor_set_,
					0u,
					0u,
					1u,
					vk::DescriptorType::eStorageBuffer,
					nullptr,
					&descriptor_buffer_info,
					nullptr
				},
			},
			{});
	}
}

GameLauncher::~GameLauncher()
{
	// Sync before destruction.
	vk_device_.waitIdle();
}

void GameLauncher::BeginFrame(const vk::CommandBuffer command_buffer, const float time_s)
{
	command_buffer.bindPipeline(vk::PipelineBindPoint::eCompute, *vk_pipeline_);

	command_buffer.bindDescriptorSets(
		vk::PipelineBindPoint::eCompute,
		*vk_pipeline_layout_,
		0u,
		1u, &*vk_descriptor_set_,
		0u, nullptr);

	Uniforms uniforms;
	uniforms.time_s= time_s;
	command_buffer.pushConstants(
		*vk_pipeline_layout_,
		vk::ShaderStageFlagBits::eCompute,
		0,
		sizeof(uniforms),
		&uniforms);

	command_buffer.dispatch(1, 1, 1);
}

void GameLauncher::EndFrame(const vk::CommandBuffer command_buffer, const vk::Image swapchain_image)
{
	const vk::BufferImageCopy copy_region(
		0u,
		viewport_size_.width, viewport_size_.height,
		vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0u, 0u, 1u),
		vk::Offset3D(0, 0, 0),
		vk::Extent3D(viewport_size_.width, viewport_size_.height, 1u));

	command_buffer.copyBufferToImage(
		*vk_game_framebuffer_buffer_,
		swapchain_image,
		vk::ImageLayout::eTransferDstOptimal,
		1u,
		&copy_region);
}

} // namespace GPUGame
