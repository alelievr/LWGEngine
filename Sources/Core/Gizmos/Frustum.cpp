#include "Frustum.hpp"

#include "Core/PrimitiveMeshFactory.hpp"

#include "Utils/Math.hpp"

using namespace LWGC;
using namespace Gizmo;

Frustum::Frustum(float fovY, float aspect, float nearPlane, float farPlane, bool wireframe, const Color & c) : GizmoBase(c)
{
	std::shared_ptr< Mesh >	frustumMesh;

	if (wireframe)
		frustumMesh = PrimitiveMeshFactory::CreateWireframeFrustum(fovY, aspect, nearPlane, farPlane);
	else
		frustumMesh = PrimitiveMeshFactory::CreateFrustum(fovY, aspect, nearPlane, farPlane);

	renderer->SetMesh(frustumMesh);

	VkPipelineRasterizationStateCreateInfo rasterState = {};
	rasterState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterState.depthClampEnable = VK_FALSE;
	rasterState.rasterizerDiscardEnable = VK_FALSE;
	rasterState.polygonMode = (wireframe) ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL;
	rasterState.lineWidth = 1.0f;
	rasterState.cullMode = VK_CULL_MODE_NONE;
	rasterState.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterState.depthBiasEnable = VK_FALSE;

	VkPipelineInputAssemblyStateCreateInfo assemblyState = {};
	assemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	assemblyState.topology = (wireframe) ? VK_PRIMITIVE_TOPOLOGY_LINE_LIST : VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	assemblyState.primitiveRestartEnable = VK_FALSE;

	material->SetRasterizationState(rasterState);
	material->SetInputAssemblyState(assemblyState);
}

Frustum::~Frustum(void)
{
}

// TODO: update mesh and upload to device
void	Frustum::SetPoint0(const glm::vec3 p) { _p0 = p; }
void	Frustum::SetPoint1(const glm::vec3 p) { _p1 = p; }

std::ostream &	operator<<(std::ostream & o, Frustum const & r)
{
	o << "tostring of the class" << std::endl;
	(void)r;
	return (o);
}
