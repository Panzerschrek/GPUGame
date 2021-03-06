#include "WindowVulkan.hpp"

namespace GPUGame
{

class GameLauncher
{
public:
	GameLauncher(WindowVulkan& window_vulkan);
	~GameLauncher();

	void BeginFrame(vk::CommandBuffer command_buffer, float time_s);
	void EndFrame(vk::CommandBuffer command_buffer, vk::Image swapchain_image);

private:
	const vk::Device vk_device_;
	const vk::Extent2D viewport_size_;

	vk::UniqueBuffer vk_game_framebuffer_buffer_;
	vk::UniqueDeviceMemory vk_game_framebuffer_buffer_memory_;

	vk::UniqueShaderModule game_shader_;

	vk::UniqueDescriptorSetLayout vk_descriptor_set_layout_;
	vk::UniquePipelineLayout vk_pipeline_layout_;
	vk::UniquePipeline vk_pipeline_;
	vk::UniqueDescriptorPool vk_descriptor_pool_;
	vk::UniqueDescriptorSet vk_descriptor_set_;
};

} // namespace GPUGame
