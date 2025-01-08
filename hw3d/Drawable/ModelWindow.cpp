#include "ModelWindow.h"
#include "Node.h"

void ModelWindow::Show(Graphics& gfx, const char* windowName, const Node& root) noexcept
{

}

void ModelWindow::ApplyParameters() noexcept
{

}

DirectX::XMMATRIX ModelWindow::GetTransform() const noexcept
{
	//assert(nullptr != m_pSelectedNode);
	//const auto& transform = m_NodesTransform.at(m_pSelectedNode->GetId()).m_transformParams;
	//return DirectX::XMMatrixRotationRollPitchYaw(transform.m_pitch, transform.m_yaw, transform.m_roll) *
	//	DirectX::XMMatrixTranslation(transform.m_x, transform.m_y, transform.m_z);

	return DirectX::XMMatrixIdentity();
}

//const DynamicData::BufferEx& ModelWindow::GetMaterial() const noexcept
//{
//	assert(nullptr != m_pSelectedNode);
//	const auto& material = m_NodesTransform.at(m_pSelectedNode->GetId()).m_materialCBuf;
//	assert(material);
//	return *material;
//}

bool ModelWindow::TransformDirty() const noexcept
{
	// return m_pSelectedNode && m_NodesTransform.at(m_pSelectedNode->GetId()).m_transformParamsDirty;
	return false;
}

void ModelWindow::ResetTransformDirty() noexcept
{
	//m_NodesTransform.at(m_pSelectedNode->GetId()).m_transformParamsDirty = false;
}

bool ModelWindow::MaterialDirty() const noexcept
{
	//return m_pSelectedNode && m_NodesTransform.at(m_pSelectedNode->GetId()).m_materialCBufDirty;
	return false;
}

void ModelWindow::ResetMaterialDirty() noexcept
{
	//m_NodesTransform.at(m_pSelectedNode->GetId()).m_materialCBufDirty = false;
}

bool ModelWindow::IsDirty() const noexcept
{
	//return TransformDirty() || MaterialDirty();
	return false;
}
