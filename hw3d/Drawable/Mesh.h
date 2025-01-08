#pragma once
#include "../Topology.h"
#include "../IndexBuffer.h"
#include "../TransformCBuf.h"

#include "../assimp/include/assimp/Importer.hpp"
#include "../assimp/include/assimp/scene.h"
#include "../assimp/include/assimp/postprocess.h"
#include <optional>

#include "../DynamicData/DynamicVertex.h"
#include "../VertexBuffer.h"
#include "../VertexShader.h"
#include "../PixelShader.h"
#include "../InputLayout.h"
#include "../Texture.h"
#include "../Sampler.h"
#include "../imgui/imgui.h"
#include "../DynamicData/DynamicConstant.h"
#include <type_traits>
#include <filesystem>
#include "Node.h"

// 模型异常类
class ModelException
{
public:

private:
	std::string m_node;
};

// 网格类
class Mesh : public Drawable
{
public:
	Mesh(Graphics& gfx, const class Material& mat, const aiMesh& mesh, float scale = 1.0f) noexcept;

	//// 传入累计的矩阵
	//void Draw(Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept;

	using Drawable::Drawable;

	DirectX::XMMATRIX GetTransformXM() const noexcept override;

	void Submit(FrameCommander& frame, DirectX::FXMMATRIX accumulatedTransform) const noexcept;

private:
	mutable DirectX::XMFLOAT4X4 m_transform; // 模型的变换矩阵
};