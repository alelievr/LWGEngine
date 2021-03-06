#include "Vk.hpp"

#include "Core/Vulkan/Material.hpp"

using namespace LWGC;

VkSampler Vk::Samplers::depthCompare;
VkSampler Vk::Samplers::trilinearClamp;
VkSampler Vk::Samplers::trilinearRepeat;
VkSampler Vk::Samplers::nearestClamp;
VkSampler Vk::Samplers::nearestRepeat;
VkSampler Vk::Samplers::anisotropicTrilinearClamp;

VkImageView		Vk::CreateImageView(VkImage image, VkFormat format, int mipLevels, VkImageViewType viewType, VkImageAspectFlags aspectFlags)
{
	VulkanInstance * instance = VulkanInstance::Get();

	VkImageViewCreateInfo viewInfo = {};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = viewType;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = aspectFlags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = mipLevels;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	VkImageView imageView;
	auto device = instance->GetDevice();
	if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture image view!");
	}

	return imageView;
}

void			Vk::CreateImage(uint32_t width, uint32_t height, uint32_t depth, int arrayCount, int mipLevels, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
{
	VulkanInstance * instance = VulkanInstance::Get();

	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = (depth == 1) ? VK_IMAGE_TYPE_2D : VK_IMAGE_TYPE_3D;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = depth;
	imageInfo.mipLevels = mipLevels;
	imageInfo.arrayLayers = arrayCount;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	// TODO: expose as parameter, currently the resource can only be used by one queue
	// (otherwise it have to trasfer the ownership of the resource)
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	auto device = instance->GetDevice();
	if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS)
	    throw std::runtime_error("failed to create image!");

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device, image, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = instance->FindMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
	    throw std::runtime_error("failed to allocate image memory!");

	CheckResult(vkBindImageMemory(device, image, imageMemory, 0), "Bind image memory failed");
}

bool			Vk::HasStencilComponent(VkFormat format)
{
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void			Vk::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer & buffer, VkDeviceMemory & bufferMemory)
{
	VulkanInstance * instance = VulkanInstance::Get();
	const auto & device = instance->GetDevice();

	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
		throw std::runtime_error("failed to create buffer!");

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = instance->FindMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
		throw std::runtime_error("failed to allocate buffer memory!");

	CheckResult(vkBindBufferMemory(device, buffer, bufferMemory, 0), "Bind Buffer Memory failed");
}

void			Vk::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	const auto & graphicCommandBufferPool = VulkanInstance::Get()->GetCommandBufferPool();
	VkCommandBuffer commandBuffer = graphicCommandBufferPool->BeginSingle();

	VkBufferCopy copyRegion = {};
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	graphicCommandBufferPool->EndSingle(commandBuffer);
}

void			Vk::CopyBufferToImage(VkBuffer buffer, VkImage image, glm::ivec3 imageSize, glm::ivec3 offset)
{
	const auto & graphicCommandBufferPool = VulkanInstance::Get()->GetCommandBufferPool();
	VkCommandBuffer commandBuffer = graphicCommandBufferPool->BeginSingle();

	VkBufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

	region.imageOffset = {offset.x, offset.y, offset.z};
	region.imageExtent = {
		static_cast<uint32_t>(imageSize.x),
		static_cast<uint32_t>(imageSize.y),
		static_cast<uint32_t>(imageSize.z)
	};

	vkCmdCopyBufferToImage(
			commandBuffer,
			buffer,
			image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&region
			);

	graphicCommandBufferPool->EndSingle(commandBuffer);
}

VkBufferView	Vk::CreateBufferView(VkBuffer buffer, VkFormat format, VkDeviceSize offset, VkDeviceSize range)
{
	VkDevice				device = VulkanInstance::Get()->GetDevice();
	VkBufferView 			pView;
	VkBufferViewCreateInfo	pCreateInfo = {};

	pCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
	pCreateInfo.buffer = buffer;
	pCreateInfo.format = format;
	pCreateInfo.offset = offset;
	pCreateInfo.range = range;

	vkCreateBufferView(device, &pCreateInfo, nullptr, &pView);
	return pView;
}


void			Vk::CheckResult(VkResult result, const std::string & errorMessage)
{
	if (result == 0)
		return;

	std::unordered_map< int, std::string > errorCodeMap;
	errorCodeMap[0] = "VK_SUCCESS";
	errorCodeMap[1] = "VK_NOT_READY";
	errorCodeMap[2] = "VK_TIMEOUT";
	errorCodeMap[3] = "VK_EVENT_SET";
	errorCodeMap[4] = "VK_EVENT_RESET";
	errorCodeMap[5] = "VK_INCOMPLETE";
	errorCodeMap[-1] = "VK_ERROR_OUT_OF_HOST_MEMORY";
	errorCodeMap[-2] = "VK_ERROR_OUT_OF_DEVICE_MEMORY";
	errorCodeMap[-3] = "VK_ERROR_INITIALIZATION_FAILED";
	errorCodeMap[-4] = "VK_ERROR_DEVICE_LOST";
	errorCodeMap[-5] = "VK_ERROR_MEMORY_MAP_FAILED";
	errorCodeMap[-6] = "VK_ERROR_LAYER_NOT_PRESENT";
	errorCodeMap[-7] = "VK_ERROR_EXTENSION_NOT_PRESENT";
	errorCodeMap[-8] = "VK_ERROR_FEATURE_NOT_PRESENT";
	errorCodeMap[-9] = "VK_ERROR_INCOMPATIBLE_DRIVER";
	errorCodeMap[-10] = "VK_ERROR_TOO_MANY_OBJECTS";
	errorCodeMap[-11] = "VK_ERROR_FORMAT_NOT_SUPPORTED";
	errorCodeMap[-12] = "VK_ERROR_FRAGMENTED_POOL";
	errorCodeMap[-1000069000] = "VK_ERROR_OUT_OF_POOL_MEMORY";
	errorCodeMap[-1000072003] = "VK_ERROR_INVALID_EXTERNAL_HANDLE";
	errorCodeMap[-1000000000] = "VK_ERROR_SURFACE_LOST_KHR";
	errorCodeMap[-1000000001] = "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
	errorCodeMap[1000001003] = "VK_SUBOPTIMAL_KHR";
	errorCodeMap[-1000001004] = "VK_ERROR_OUT_OF_DATE_KHR";
	errorCodeMap[-1000003001] = "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
	errorCodeMap[-1000011001] = "VK_ERROR_VALIDATION_FAILED_EXT";
	errorCodeMap[-1000012000] = "VK_ERROR_INVALID_SHADER_NV";
	errorCodeMap[-1000158000] = "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
	errorCodeMap[-1000161000] = "VK_ERROR_FRAGMENTATION_EXT";
	errorCodeMap[-1000174001] = "VK_ERROR_NOT_PERMITTED_EXT";
	errorCodeMap[-1000244000] = "VK_ERROR_INVALID_DEVICE_ADDRESS_EXT";
	errorCodeMap[-1000255000] = "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
	errorCodeMap[-1000069000] = "VK_ERROR_OUT_OF_POOL_MEMORY_KHR";
	errorCodeMap[-1000072003] = "VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR";

	std::cout << "Vulkan error [" << errorCodeMap[result] << "]: " << errorMessage << std::endl;

    if (result < 0)
		abort();
}

VkSampler		Vk::CreateSampler(VkFilter filter, VkSamplerAddressMode addressMode, uint32_t maxAniso)
{
	VkSampler			sampler;
	VkSamplerCreateInfo	samplerInfo = {};

	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = filter;
	samplerInfo.minFilter = filter;
	samplerInfo.addressModeU = addressMode;
	samplerInfo.addressModeV = addressMode;
	samplerInfo.addressModeW = addressMode;
	samplerInfo.anisotropyEnable = maxAniso != 0;
	samplerInfo.maxAnisotropy = maxAniso;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 16.0f;

	if (vkCreateSampler(VulkanInstance::Get()->GetDevice(), &samplerInfo, nullptr, &sampler) != VK_SUCCESS)
		throw std::runtime_error("failed to create texture sampler!");

	return sampler;
}

VkSampler	Vk::CreateCompSampler(VkFilter filter, VkSamplerAddressMode addressMode, VkCompareOp compareOp)
{

	VkSampler			sampler;
	VkSamplerCreateInfo	samplerInfo = {};

	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = filter;
	samplerInfo.minFilter = filter;
	samplerInfo.addressModeU = addressMode;
	samplerInfo.addressModeV = addressMode;
	samplerInfo.addressModeW = addressMode;
	samplerInfo.anisotropyEnable = false;
	samplerInfo.maxAnisotropy = 0;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_TRUE;
	samplerInfo.compareOp = compareOp;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 16.0f;

	if (vkCreateSampler(VulkanInstance::Get()->GetDevice(), &samplerInfo, nullptr, &sampler) != VK_SUCCESS)
		throw std::runtime_error("failed to create texture sampler!");

	return sampler;
}

void			Vk::Initialize(void)
{
	Samplers::trilinearClamp = CreateSampler(VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER, 0);
	Samplers::trilinearRepeat = CreateSampler(VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, 0);
	Samplers::nearestClamp = CreateSampler(VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER, 0);
	Samplers::nearestRepeat = CreateSampler(VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_REPEAT, 0);
	Samplers::anisotropicTrilinearClamp = CreateSampler(VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER, 16);
	Samplers::depthCompare = CreateCompSampler(VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER, VK_COMPARE_OP_LESS);
}

void			Vk::Release(void)
{
	VkDevice	device = VulkanInstance::Get()->GetDevice();

	vkDestroySampler(device, Samplers::trilinearClamp, nullptr);
	vkDestroySampler(device, Samplers::trilinearRepeat, nullptr);
	vkDestroySampler(device, Samplers::nearestClamp, nullptr);
	vkDestroySampler(device, Samplers::nearestRepeat, nullptr);
	vkDestroySampler(device, Samplers::anisotropicTrilinearClamp, nullptr);
	vkDestroySampler(device, Samplers::depthCompare, nullptr);
}

VkDescriptorSetLayoutBinding	Vk::CreateDescriptorSetLayoutBinding(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlagBits stageFlags)
{
	VkDescriptorSetLayoutBinding layoutBinding = {};
	layoutBinding.binding = binding;
	layoutBinding.descriptorCount = 1;
	layoutBinding.descriptorType = descriptorType;
	layoutBinding.pImmutableSamplers = nullptr;
	layoutBinding.stageFlags = stageFlags;

	return layoutBinding;
}

void			Vk::CreateDescriptorSetLayout(std::vector< VkDescriptorSetLayoutBinding > bindings, VkDescriptorSetLayout & layout)
{
	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(VulkanInstance::Get()->GetDevice(), &layoutInfo, nullptr, &layout) != VK_SUCCESS)
	    throw std::runtime_error("failed to create descriptor set layout!");
}

void			Vk::UploadToMemory(VkDeviceMemory memory, void * data, size_t size, size_t offset, bool forceFlush)
{
	void *			tmpData;
	VkDevice		device = VulkanInstance::Get()->GetDevice();
	const auto &	limits = VulkanInstance::Get()->GetLimits();
	size_t			alignedOffset = offset;
	size_t			alignedSize = size;

	// Patch the offset and size if it's not memory aligned
	if ((offset % limits.nonCoherentAtomSize) != 0 || (size % limits.nonCoherentAtomSize) != 0)
	{
		alignedOffset -= offset % limits.nonCoherentAtomSize;
		alignedSize = limits.nonCoherentAtomSize;
	}

	// Map aligned size and offset
	vkMapMemory(device, memory, alignedOffset, alignedSize, 0, &tmpData);
	memcpy(reinterpret_cast< char * >(tmpData) + (offset - alignedOffset), data, size);

	if (forceFlush)
	{
		VkMappedMemoryRange flush = {};
		flush.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		flush.memory = memory;
		flush.offset = alignedOffset;
		flush.size = alignedSize;

		vkFlushMappedMemoryRanges(device, 1, &flush);
	}

	vkUnmapMemory(device, memory);
}

void			Vk::SetDebugName(const std::string & name, uint64_t vulkanObject, VkDebugReportObjectTypeEXT objectType)
{
	if (!VulkanInstance::AreDebugMarkersEnabled())
		return ;

	if (vulkanObject == VK_NULL_HANDLE)
	{
		std::cerr << "ERROR: Trying to set debug name on null vulkan object: " << name << std::endl;
		return;
	}

// We don't enable this for mac because the marker extension is not well supported
#ifndef __unix__
	return ;
#endif

	VkDevice	device = VulkanInstance::Get()->GetDevice();

	VkDebugMarkerObjectNameInfoEXT nameInfo = {};
	nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_NAME_INFO_EXT;
	nameInfo.objectType = objectType;
	nameInfo.object = vulkanObject;
	nameInfo.pObjectName = name.c_str();

	Vk::CheckResult(vkDebugMarkerSetObjectNameEXT(device, &nameInfo), "Failed to set debug marker on object " + name);
}

// Utils functions to directly set vulkan objects without specifying the debug object type:
#define VULKAN_DEBUG_NAME_FUNCTION_TEMPLATE(functionName, vulkanObjectType, debugObjectType) \
void			Vk::functionName(const std::string & name, vulkanObjectType obj) \
{ \
	SetDebugName(name, reinterpret_cast<uint64_t>(obj), debugObjectType); \
}

VULKAN_DEBUG_NAME_FUNCTION_TEMPLATE(SetImageDebugName, VkImage, VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_EXT)
VULKAN_DEBUG_NAME_FUNCTION_TEMPLATE(SetImageViewDebugName, VkImageView, VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_VIEW_EXT)
VULKAN_DEBUG_NAME_FUNCTION_TEMPLATE(SetCommandBufferDebugName, VkCommandBuffer, VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_BUFFER_EXT)
VULKAN_DEBUG_NAME_FUNCTION_TEMPLATE(SetQueueDebugName, VkQueue, VK_DEBUG_REPORT_OBJECT_TYPE_QUEUE_EXT)
VULKAN_DEBUG_NAME_FUNCTION_TEMPLATE(SetSamplerDebugName, VkSampler, VK_DEBUG_REPORT_OBJECT_TYPE_SAMPLER_EXT)
VULKAN_DEBUG_NAME_FUNCTION_TEMPLATE(SetDeviceMemoryDebugName, VkDeviceMemory, VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_MEMORY_EXT)
VULKAN_DEBUG_NAME_FUNCTION_TEMPLATE(SetShaderModuleDebugName, VkShaderModule, VK_DEBUG_REPORT_OBJECT_TYPE_SHADER_MODULE_EXT)
VULKAN_DEBUG_NAME_FUNCTION_TEMPLATE(SetPipelineDebugName, VkPipeline, VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_EXT)
VULKAN_DEBUG_NAME_FUNCTION_TEMPLATE(SetPipelineLayoutDebugName, VkPipelineLayout, VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_LAYOUT_EXT)
VULKAN_DEBUG_NAME_FUNCTION_TEMPLATE(SetRenderPassDebugName, VkRenderPass, VK_DEBUG_REPORT_OBJECT_TYPE_RENDER_PASS_EXT)
VULKAN_DEBUG_NAME_FUNCTION_TEMPLATE(SetFramebufferDebugName, VkFramebuffer, VK_DEBUG_REPORT_OBJECT_TYPE_FRAMEBUFFER_EXT)
VULKAN_DEBUG_NAME_FUNCTION_TEMPLATE(SetDescriptorSetLayoutDebugName, VkDescriptorSetLayout, VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT_EXT)
VULKAN_DEBUG_NAME_FUNCTION_TEMPLATE(SetDescriptorSetDebugName, VkDescriptorSet, VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_EXT)
VULKAN_DEBUG_NAME_FUNCTION_TEMPLATE(SetSemaphoreDebugName, VkSemaphore, VK_DEBUG_REPORT_OBJECT_TYPE_SEMAPHORE_EXT)
VULKAN_DEBUG_NAME_FUNCTION_TEMPLATE(SetFenceDebugName, VkFence, VK_DEBUG_REPORT_OBJECT_TYPE_FENCE_EXT)
VULKAN_DEBUG_NAME_FUNCTION_TEMPLATE(SetEventDebugName, VkEvent, VK_DEBUG_REPORT_OBJECT_TYPE_EVENT_EXT)

#undef VULKAN_DEBUG_NAME_FUNCTION_TEMPLATE

void			Vk::BeginProfilingSample(VkCommandBuffer cmd, const std::string & debugSampleName, const Color & color)
{
	if (!VulkanInstance::AreDebugMarkersEnabled())
		return;

	// We don't enable this because the marker extension doesn't work with renderdoc
	return ;

	VkDebugMarkerMarkerInfoEXT markerInfo = {};
	markerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT;
	memcpy(markerInfo.color, &color.r, sizeof(float) * 4);
	markerInfo.pMarkerName = debugSampleName.c_str();

	vkCmdDebugMarkerBeginEXT(cmd, &markerInfo);
}

void			Vk::InsertProfilingSample(VkCommandBuffer cmd, const std::string & debugSampleName, const Color & color)
{
	if (!VulkanInstance::AreDebugMarkersEnabled())
		return;

	// We don't enable this because the marker extension doesn't work with renderdoc
	return ;

	VkDebugMarkerMarkerInfoEXT markerInfo = {};
	markerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT;
	memcpy(markerInfo.color, &color.r, sizeof(float) * 4);
	markerInfo.pMarkerName = debugSampleName.c_str();
	vkCmdDebugMarkerInsertEXT(cmd, &markerInfo);
}

void			Vk::EndProfilingSample(VkCommandBuffer cmd)
{
	if (!VulkanInstance::AreDebugMarkersEnabled())
		return ;

	// We don't enable this because the marker extension doesn't work with renderdoc
	return ;

	vkCmdDebugMarkerEndEXT(cmd);
}

VkFence			Vk::CreateFence(bool signaled)
{
	VkDevice			device = VulkanInstance::Get()->GetDevice();
	VkFence				fence;
	VkFenceCreateInfo	fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = (signaled) ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

	vkCreateFence(device, &fenceInfo, nullptr, &fence);
	return fence;
}

uint64_t		Vk::CreateAccelerationStructure(const VkAccelerationStructureTypeNV type, const uint32_t geometryCount, const VkGeometryNV * geometries, const uint32_t instanceCount)
{
	VulkanInstance *			instance = VulkanInstance::Get();
	VkDevice					device = instance->GetDevice();
	VkAccelerationStructureNV	accelerationStructure;
	VkDeviceMemory				accelerationStructureMemory;

	VkAccelerationStructureInfoNV	accelerationStructureInfo = {};
    accelerationStructureInfo.type = type;
    accelerationStructureInfo.flags = 0;
    accelerationStructureInfo.instanceCount = instanceCount;
    accelerationStructureInfo.geometryCount = geometryCount;
    accelerationStructureInfo.pGeometries = geometries;

	VkAccelerationStructureCreateInfoNV accelerationStructureCreateInfo = {};
    accelerationStructureCreateInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_NV;
    accelerationStructureCreateInfo.info = accelerationStructureInfo;
    accelerationStructureCreateInfo.compactedSize = 0;

    Vk::CheckResult(vkCreateAccelerationStructureNV(device,
		&accelerationStructureCreateInfo, nullptr, &accelerationStructure),
		"Failed to create acceleration structure"
	);

    VkAccelerationStructureMemoryRequirementsInfoNV memoryRequirementsInfo;
    memoryRequirementsInfo.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_MEMORY_REQUIREMENTS_INFO_NV;
    memoryRequirementsInfo.pNext = nullptr;
    memoryRequirementsInfo.accelerationStructure = accelerationStructure;

    VkMemoryRequirements2 memoryRequirements;
    vkGetAccelerationStructureMemoryRequirementsNV(device, &memoryRequirementsInfo, &memoryRequirements);

    VkMemoryAllocateInfo memoryAllocateInfo;
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.pNext = nullptr;
    memoryAllocateInfo.allocationSize = memoryRequirements.memoryRequirements.size;
    memoryAllocateInfo.memoryTypeIndex = instance->FindMemoryType(memoryRequirements.memoryRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    Vk::CheckResult(vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &accelerationStructureMemory),
        "Failed to allocate memory for acceleration structure"
	);

    VkBindAccelerationStructureMemoryInfoNV bindInfo;
    bindInfo.sType = VK_STRUCTURE_TYPE_BIND_ACCELERATION_STRUCTURE_MEMORY_INFO_NV;
    bindInfo.pNext = nullptr;
    bindInfo.accelerationStructure = accelerationStructure;
    bindInfo.memory = accelerationStructureMemory;
    bindInfo.memoryOffset = 0;
    bindInfo.deviceIndexCount = 0;
    bindInfo.pDeviceIndices = nullptr;

    Vk::CheckResult(vkBindAccelerationStructureMemoryNV(device, 1, &bindInfo),
        "Failed to bind the acceleration structure memory"
	);

	uint64_t handle = -1;
    Vk::CheckResult(vkGetAccelerationStructureHandleNV(device, accelerationStructure, sizeof(uint64_t), &handle),
		"Failed to get the acceleration structure handle"
	);

	return handle;
}
