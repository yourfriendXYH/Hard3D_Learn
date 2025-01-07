#include "Mesh.h"
#include "../imgui/imgui.h"
#include <unordered_map>
#include "../Surface.h"
#include <stdexcept>
#include "../Bindable/Blender.h"
#include "../Bindable/Rasterizer.h"
#include "../Bindable/ConstantBuffersEx.h"
#include "../DynamicData/DynamicConstant.h"
#include "../DynamicData/LayoutCodex.h"
#include "../Bindable/Stencil.h"
#include "../Bindable/FrameCommander.h"
#include "Material.h"

Mesh::Mesh(Graphics& gfx, const Material& mat, const aiMesh& mesh) noexcept
	:
	Drawable(gfx, mat, mesh)
{

}

DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat4x4(&m_transform);
}

void Mesh::Submit(FrameCommander& frame, DirectX::FXMMATRIX accumulatedTransform) const noexcept
{
	DirectX::XMStoreFloat4x4(&m_transform, accumulatedTransform);
	Drawable::Submit(frame);
}
