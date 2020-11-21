#include "GameLauncher.hpp"
#include "Assert.hpp"


namespace GPUGame
{

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
	{// Fill buffer with initial values.
		void* buffer_data_mapped= nullptr;
		vk_device_.mapMemory(*vk_game_framebuffer_buffer_memory_, 0u, viewport_size_.width * viewport_size_.height * 4, vk::MemoryMapFlags(), &buffer_data_mapped);

		for(uint32_t y= 0u; y < viewport_size_.height; ++y)
		for(uint32_t x= 0u; x < viewport_size_. width; ++x)
		{
			const uint8_t r= uint8_t(x * 255u / viewport_size_.width );
			const uint8_t g= uint8_t(y * 255u / viewport_size_.height);
			const uint8_t b= 0u;
			const uint8_t a= 0u;

			uint8_t* const dst= reinterpret_cast<uint8_t*>(buffer_data_mapped) + 4u * (x + y * viewport_size_.width);
			dst[0]= b;
			dst[1]= g;
			dst[2]= r;
			dst[3]= a;
		}

		vk_device_.unmapMemory(*vk_game_framebuffer_buffer_memory_);
	}
}

GameLauncher::~GameLauncher()
{
	// Sync before destruction.
	vk_device_.waitIdle();
}

void GameLauncher::BeginFrame(const vk::CommandBuffer command_buffer)
{
	GG_UNUSED(command_buffer);
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
