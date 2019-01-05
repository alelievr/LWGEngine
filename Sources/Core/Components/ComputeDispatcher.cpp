#include "ComputeDispatcher.hpp"

#include "Core/Rendering/VulkanRenderPipeline.hpp"
#include "Core/Hierarchy.hpp"

using namespace LWGC;

ComputeDispatcher::ComputeDispatcher(Material * material, int width, int height, int depth) :
	_material(material), _width(width), _height(height), _depth(depth)
{
}

ComputeDispatcher::~ComputeDispatcher(void)
{

}

void	ComputeDispatcher::RecordComputeCommand(VkCommandBuffer cmd) noexcept
{
	vkCmdDispatch(cmd, _width, _height, _depth);
}

void			ComputeDispatcher::Initialize(void) noexcept
{
	Component::Initialize();

	_material->MarkAsReady();

	_computeCommandBuffer = VulkanInstance::Get()->GetCommandBufferPool()->Allocate(VK_COMMAND_BUFFER_LEVEL_SECONDARY);

	// Record command buffer:
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;

	_material->GetComputeWorkSize(_workGroupWidth, _workGroupHeight, _workGroupDepth);

	if (_width % _workGroupWidth != 0 || _height % _workGroupHeight != 0 || _depth % _workGroupDepth != 0)
	{
		std::cout << "Dispatch size for compute shader is not multiple of his work group size: "
			<< _workGroupWidth << ", " << _workGroupHeight << ", " << _workGroupDepth << std::endl;
		return ;
	}

	if (vkBeginCommandBuffer(_computeCommandBuffer, &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	vkCmdBindPipeline(_computeCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, _material->GetPipeline());

	vkCmdDispatch(_computeCommandBuffer, _width / _workGroupWidth, _height / _workGroupHeight, _depth / _workGroupDepth);

	if (vkEndCommandBuffer(_computeCommandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to record command buffer!");
	}
}

void			ComputeDispatcher::OnEnable() noexcept
{
	Component::OnEnable();
	_renderContextIndex = hierarchy->RegisterComponentInRenderContext(GetType(), this);
}

void			ComputeDispatcher::OnDisable() noexcept
{
	Component::OnDisable();
	hierarchy->UnregisterComponentInRenderContext(GetType(), _renderContextIndex);
}

Material *	ComputeDispatcher::GetMaterial(void)
{
	return _material;
}

VkCommandBuffer				ComputeDispatcher::GetCommandBuffer(void)
{
	return _computeCommandBuffer;
}

uint32_t		ComputeDispatcher::GetType(void) const noexcept
{
	return static_cast< uint32_t >(ComponentType::ComputeDispatcher);
}

std::ostream &	operator<<(std::ostream & o, ComputeDispatcher const & r)
{
	o << "ComputeDispatcher" << std::endl;
	(void)r;
	return (o);
}
