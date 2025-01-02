#include "Material.h"
#include "../DynamicData/DynamicConstant.h"
#include "../Texture.h"
#include "../Bindable/Rasterizer.h"
#include "../TransformCBuf.h"
#include "../Bindable/Blender.h"
#include "../VertexShader.h"
#include "../PixelShader.h"
#include "../InputLayout.h"
#include "../Sampler.h"
#include "../Bindable/ConstantBuffersEx.h"

Material::Material(Graphics& gfx, const aiMaterial& material, const std::filesystem::path& path) noexcept
	:
	m_modelPath(path.string())
{
	const auto rootPath = path.parent_path().string() + "\\";
	{
		aiString tempName;
		material.Get(AI_MATKEY_NAME, tempName);
		m_name = tempName.C_Str();
	}
	// phong technique
	{
		Technique phong{ "Phong" };
		Step step(0);
		std::string shaderCode = "Phong";
		aiString texFileName;
		m_vtxLayout.Append(DynamicData::VertexLayout::Position3D);
		m_vtxLayout.Append(DynamicData::VertexLayout::Normal);
		DynamicData::RawLayoutEx pscLayout;
		bool hasTexture = false;
		bool hasGlossAlpha = false;

		// 材质属性相关的管线数据
		// diffuse（漫反射纹理数据）
		{
			bool hasAlpha = false;
			if (material.GetTexture(aiTextureType_DIFFUSE, 0, &texFileName) == aiReturn_SUCCESS)
			{
				hasTexture = true;
				shaderCode += "Dif";
				m_vtxLayout.Append(DynamicData::VertexLayout::Texture2D);
				auto pTexture = Bind::Texture::Resolve(gfx, rootPath + texFileName.C_Str());
				if (pTexture->HasAlpha())
				{
					hasAlpha = true;
					shaderCode += "Msk";
				}
				step.AddBindable(std::move(pTexture));
			}
			else
			{
				pscLayout.Add<DynamicData::EFloat3>("materialColor");
			}
			step.AddBindable(Bind::Rasterizer::Resolve(gfx, hasAlpha));
		}
		// specular（高光纹理数据）
		{
			if (material.GetTexture(aiTextureType_SPECULAR, 0, &texFileName) == aiReturn_SUCCESS)
			{
				hasTexture = true;
				shaderCode += "Spc";
				m_vtxLayout.Append(DynamicData::VertexLayout::Texture2D);
				auto pTexture = Bind::Texture::Resolve(gfx, rootPath + texFileName.C_Str(), 1u);
				hasGlossAlpha = pTexture->HasAlpha();
				step.AddBindable(std::move(pTexture));
				pscLayout.Add<DynamicData::EBool>("useGlossAlpha");
			}
			pscLayout.Add<DynamicData::EFloat3>("specularColor");
			pscLayout.Add<DynamicData::EFloat>("specularWeight");
			pscLayout.Add<DynamicData::EFloat>("specularGloss");
		}
		// normal（法线纹理数据）
		{
			if (material.GetTexture(aiTextureType_NORMALS, 0, &texFileName) == aiReturn_SUCCESS)
			{
				hasTexture = true;
				shaderCode += "Nrm";
				m_vtxLayout.Append(DynamicData::VertexLayout::Texture2D);
				m_vtxLayout.Append(DynamicData::VertexLayout::Tangent);
				m_vtxLayout.Append(DynamicData::VertexLayout::Bitangent);
				step.AddBindable(Bind::Texture::Resolve(gfx, rootPath + texFileName.C_Str(), 2u));
				pscLayout.Add<DynamicData::EBool>("useNormalMap");
				pscLayout.Add<DynamicData::EFloat>("normalMapWeight");
			}
		}

		// 通用管线数据
		// common（post）
		{
			step.AddBindable(std::make_shared<Bind::TransformCBuf>(gfx, 0u));
			step.AddBindable(Bind::Blender::Resolve(gfx, false));
			auto pvs = Bind::VertexShader::Resolve(gfx, shaderCode + "_VS.cso");
			auto pvsbc = pvs->GetByteCode();
			step.AddBindable(std::move(pvs));
			step.AddBindable(Bind::PixelShader::Resolve(gfx, shaderCode + "_PS.cso"));
			step.AddBindable(Bind::InputLayout::Resolve(gfx, m_vtxLayout, pvsbc));
			if (hasTexture)
			{
				step.AddBindable(Bind::Sampler::Resolve(gfx)); // 纹理采样器
			}

			// 顶点着色器的常数缓存（材质参数）
			DynamicData::BufferEx buf{ std::move(pscLayout) };
			if (DynamicData::ElementRefEx result = buf["materialColor"]; result.Exists())
			{
				aiColor3D color = { 0.45f, 0.45f, 0.85f };
				material.Get(AI_MATKEY_COLOR_DIFFUSE, color);
				result = reinterpret_cast<DirectX::XMFLOAT3&>(color);
			}

			buf["useGlossAlpha"].SetIfExists(hasGlossAlpha);
			if (auto r = buf["specularColor"]; r.Exists())
			{
				aiColor3D color = { 0.18f, 0.18f, 0.18f };
				material.Get(AI_MATKEY_COLOR_SPECULAR, color);
				r = reinterpret_cast<DirectX::XMFLOAT3&>(color);
			}
			buf["specularWeight"].SetIfExists(1.0f);
			if (auto r = buf["specularGloss"]; r.Exists())
			{
				float gloss = 8.0f;
				material.Get(AI_MATKEY_SHININESS, gloss);
				r = gloss;
			}
			buf["useNormalMap"].SetIfExists(true);
			buf["normalMapWeight"].SetIfExists(1.0f);
			step.AddBindable(std::make_shared<Bind::CachingPixelConstantBufferEx>(gfx, std::move(buf), 1u));
		}

		// 描边特效 Outline Technique
		{
			Technique outline("Outline");
			{
				Step mask(1u);

				auto pvs = Bind::VertexShader::Resolve(gfx, "Solid_VS.cso");
				auto pvsbc = pvs->GetByteCode();
				mask.AddBindable(std::move(pvs));
				mask.AddBindable(Bind::InputLayout::Resolve(gfx, m_vtxLayout, pvsbc));
				mask.AddBindable(std::make_shared<Bind::TransformCBuf>(gfx));

				outline.AddStep(std::move(mask));
			}
			{
				Step draw(2u);
				auto pvs = Bind::VertexShader::Resolve(gfx, "Solid_VS.cso");
				auto pvsbc = pvs->GetByteCode();
				draw.AddBindable(std::move(pvs));
				draw.AddBindable(Bind::PixelShader::Resolve(gfx, "Solid_PS.cso"));

				DynamicData::RawLayoutEx layout;
				layout.Add<DynamicData::EFloat3>("color");
				auto buf = DynamicData::BufferEx(std::move(layout));
				buf["color"] = DirectX::XMFLOAT3{ 0.4f, 0.4f, 1.0f };
				draw.AddBindable(std::make_shared<Bind::CachingPixelConstantBufferEx>(gfx, buf, 1u));

				draw.AddBindable(Bind::InputLayout::Resolve(gfx, m_vtxLayout, pvsbc));

				class TransformCBufScaling : public Bind::TransformCBuf
				{
				public:
					TransformCBufScaling(Graphics& gfx, float scale = 1.04f)
						:
						TransformCBuf(gfx),
						m_buf(MakeLayout())
					{
						m_buf["scale"] = scale;
					}
					void Bind(Graphics& gfx) noexcept override
					{
						float scale = m_buf["scale"];
						const auto scaleMatrix = DirectX::XMMatrixScaling(scale, scale, scale);
						auto tempTransform = GetTransforms(gfx);
						tempTransform.modelView = tempTransform.modelView * scaleMatrix;
						tempTransform.modelViewProj = tempTransform.modelViewProj * scaleMatrix;
						UpdateBindImpl(gfx, tempTransform);
					}

					void Accept(TechniqueProbe& probe)
					{
						probe.VisitBuffer(m_buf);
					}

				private:
					static DynamicData::RawLayoutEx MakeLayout()
					{
						DynamicData::RawLayoutEx layout;
						layout.Add<DynamicData::EFloat>("scale");
						return layout;
					}

				private:
					DynamicData::BufferEx m_buf;
				};
				draw.AddBindable(std::make_shared<TransformCBufScaling>(gfx));

				outline.AddStep(std::move(draw));
			}
			m_techniques.emplace_back(std::move(outline));
		}

		phong.AddStep(std::move(step));

		m_techniques.push_back(std::move(phong));

	}

}

DynamicData::VerticesBuffer Material::ExtractVertices(const aiMesh& mesh) const noexcept
{
	return { m_vtxLayout, mesh };
}

std::vector<unsigned short> Material::ExtractIndices(const aiMesh& mesh) const noexcept
{
	std::vector<unsigned short> indices;
	indices.reserve(mesh.mNumFaces * 3);
	for (unsigned int i = 0u; i < mesh.mNumFaces; ++i)
	{
		const auto& face = mesh.mFaces[i];
		assert(face.mNumIndices == 3);
		indices.emplace_back(face.mIndices[0]);
		indices.emplace_back(face.mIndices[1]);
		indices.emplace_back(face.mIndices[2]);
	}
	return indices;
}

std::shared_ptr<Bind::VertexBuffer> Material::MakeVertexBindable(Graphics& gfx, const aiMesh& mesh) const noexcept
{
	return Bind::VertexBuffer::Resolve(gfx, MakeMeshTag(mesh), ExtractVertices(mesh));
}

std::shared_ptr<Bind::IndexBuffer> Material::MakeIndexBindable(Graphics& gfx, const aiMesh& mesh) const noexcept
{
	return Bind::IndexBuffer::Resolve(gfx, MakeMeshTag(mesh), ExtractIndices(mesh));
}

std::vector<Technique> Material::GetTechniques() const noexcept
{
	return m_techniques;
}

std::string Material::MakeMeshTag(const aiMesh& mesh) const noexcept
{
	return m_modelPath + "%" + mesh.mName.C_Str();
}
