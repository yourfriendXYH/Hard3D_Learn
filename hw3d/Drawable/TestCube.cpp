#include "TestCube.h"
#include "../Geometry/Cube.h"
#include "../VertexBuffer.h"
#include "../IndexBuffer.h"
#include "../Texture.h"
#include "../Bindable/TransformCBufDoubleboi.h"
#include "../VertexShader.h"
#include "../PixelShader.h"
#include "../InputLayout.h"
#include "../Topology.h"
#include "../imgui/imgui.h"
#include "../Bindable/Stencil.h"
#include "../Sampler.h"
#include "../DynamicData/DynamicConstant.h"
#include "../Bindable/ConstantBuffersEx.h"

TestCube::TestCube(Graphics& gfx, float size)
{
	using namespace Bind;

	auto model = Cube::MakeIndependentTextured();
	model.Transform(DirectX::XMMatrixScaling(size, size, size));
	model.SetNormalsIndependentFlat();

	const auto geomtryTag = "$cube." + std::to_string(size);

	m_pVertexBuffer = VertexBuffer::Resolve(gfx, geomtryTag, model.m_vertices);
	m_pIndexBuffer = IndexBuffer::Resolve(gfx, geomtryTag, model.m_indices);
	m_pTopology = Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	{
		Technique standard("Shade");
		{
			Step only(0u);
			only.AddBindable(Texture::Resolve(gfx, "Resources/Wall/brickwall.jpg"));
			only.AddBindable(Sampler::Resolve(gfx));

			auto pvs = VertexShader::Resolve(gfx, "PhongVS.cso");
			auto pvsbc = pvs->GetByteCode();
			only.AddBindable(std::move(pvs));
			only.AddBindable(PixelShader::Resolve(gfx, "PhongPS.cso"));

			DynamicData::RawLayoutEx layout;
			layout.Add<DynamicData::EFloat>("specularIntensity");
			layout.Add<DynamicData::EFloat>("specularPower");
			DynamicData::BufferEx buf = DynamicData::BufferEx(std::move(layout));
			buf["specularIntensity"] = 0.1f;
			buf["specularPower"] = 20.0f;

			only.AddBindable(std::make_shared<CachingPixelConstantBufferEx>(gfx, buf, 1u));
			//only.AddBindable(PixelConstantBuffer<PSMaterialConstant>::Resolve(gfx, m_pmc, 1u));
			only.AddBindable(InputLayout::Resolve(gfx, model.m_vertices.GetVertexLayout(), pvsbc));
			only.AddBindable(std::make_shared<TransformCBuf>(gfx));

			standard.AddStep(only);
		}
		AddTechnique(std::move(standard));
	}

	{
		Technique outline("Outline");
		{
			Step mask(1u);

			auto pvs = VertexShader::Resolve(gfx, "SolidVS.cso");
			auto pvsbc = pvs->GetByteCode();
			mask.AddBindable(std::move(pvs));
			mask.AddBindable(InputLayout::Resolve(gfx, model.m_vertices.GetVertexLayout(), pvsbc));
			mask.AddBindable(std::make_shared<TransformCBuf>(gfx));

			outline.AddStep(std::move(mask));
		}
		{
			Step draw(2u);
			auto pvs = VertexShader::Resolve(gfx, "SolidVS.cso");
			auto pvsbc = pvs->GetByteCode();
			draw.AddBindable(std::move(pvs));
			draw.AddBindable(PixelShader::Resolve(gfx, "SolidPS.cso"));

			DynamicData::RawLayoutEx layout;
			layout.Add<DynamicData::EFloat4>("color");
			auto buf = DynamicData::BufferEx(std::move(layout));
			buf["color"] = DirectX::XMFLOAT4{0.4f, 0.4f, 1.0f, 1.0f};
			draw.AddBindable(std::make_shared<CachingPixelConstantBufferEx>(gfx, buf, 1u));

			draw.AddBindable(InputLayout::Resolve(gfx, model.m_vertices.GetVertexLayout(), pvsbc));

			class TransformCBufScaling : public TransformCBuf
			{
			public:
				TransformCBufScaling(Graphics& gfx, float scale = 1.04f)
					:
					TransformCBuf(gfx),
					m_buf(MakeLayout())
				{

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
		AddTechnique(std::move(outline));
	}

	//AddBind(VertexBuffer::Resolve(gfx, geomtryTag, model.m_vertices));
	//AddBind(IndexBuffer::Resolve(gfx, geomtryTag, model.m_indices));

	//AddBind(Texture::Resolve(gfx, "Resources/Wall/brickwall.jpg"));
	////AddBind(Texture::Resolve(gfx, "Resources/Wall/brickwall_normal.jpg", 1u));
	//AddBind(Sampler::Resolve(gfx));

	//auto pvs = VertexShader::Resolve(gfx, "PhongVS.cso");
	//auto pvsbc = pvs->GetByteCode();
	//AddBind(std::move(pvs));

	//AddBind(PixelShader::Resolve(gfx, "PhongPS.cso"));

	//AddBind(PixelConstantBuffer<PSMaterialConstant>::Resolve(gfx, m_pmc, 1u));

	//AddBind(InputLayout::Resolve(gfx, model.m_vertices.GetVertexLayout(), pvsbc));

	//AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	//auto tcbdb = std::make_shared<TransformCBufDoubleboi>(gfx, *this, 0u, 2u);

	//AddBind(tcbdb);
	//// 写入模板标记
	//AddBind(std::make_shared<Stencil>(gfx, Stencil::Mode::Write));

	// 准备描边效果的管线数据
	//m_outlineEffect.emplace_back(VertexBuffer::Resolve(gfx, geomtryTag, model.m_vertices));
	//m_outlineEffect.emplace_back(IndexBuffer::Resolve(gfx, geomtryTag, model.m_indices));
	//pvs = VertexShader::Resolve(gfx, "SolidVS.cso");
	//pvsbc = pvs->GetByteCode();
	//m_outlineEffect.emplace_back(std::move(pvs));
	//m_outlineEffect.emplace_back(PixelShader::Resolve(gfx, "SolidPS.cso"));
	//struct SolidColorBuffer
	//{
	//	DirectX::XMFLOAT4 m_outlineColor = { 1.0f, 0.4f, 0.4f, 1.0f };
	//}outlinaPixelBuffer;
	//m_outlineEffect.emplace_back(PixelConstantBuffer<SolidColorBuffer>::Resolve(gfx, outlinaPixelBuffer, 1u));
	//m_outlineEffect.emplace_back(InputLayout::Resolve(gfx, model.m_vertices.GetVertexLayout(), pvsbc));
	//m_outlineEffect.emplace_back(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	//m_outlineEffect.emplace_back(std::move(tcbdb));
	//m_outlineEffect.emplace_back(std::make_shared<Stencil>(gfx, Stencil::Mode::Mask));
}

void TestCube::SetPos(DirectX::XMFLOAT3 pos) noexcept
{
	m_pos = pos;
}

void TestCube::SetRotation(float roll, float pitch, float yaw) noexcept
{
	m_roll = roll;
	m_pitch = pitch;
	m_yaw = yaw;
}

DirectX::XMMATRIX TestCube::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, m_roll) *
		DirectX::XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);
}

void TestCube::SpawnControlWindow(Graphics& gfx, const char* name) noexcept
{
	if (ImGui::Begin(name))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &m_pos.x, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Y", &m_pos.y, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Z", &m_pos.z, -80.0f, 80.0f, "%.1f");
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Roll", &m_roll, -180.0f, 180.0f);
		ImGui::SliderAngle("Pitch", &m_pitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &m_yaw, -180.0f, 180.0f);

		class Probe : public TechniqueProbe
		{
		public:
			void OnSetTechnique() override
			{
				using namespace std::string_literals;
				ImGui::TextColored({ 0.4f, 1.0f, 0.6f, 1.0f }, m_pTechnique->GetName().c_str()); // 显示Technique的名字
				bool active = m_pTechnique->IsActive();
				ImGui::Checkbox(("Tech Active##"s + /*m_pTechnique->GetName()*/std::to_string(m_techIndex)).c_str(), &active);
				m_pTechnique->SetActiveState(active);
			}
			bool OnVisitBuffer(DynamicData::BufferEx& buf) override
			{
				bool dirty = false;
				// 当前UI被修改或前面的UI被修改，都置脏
				const auto dirtyCheck = [&dirty](bool changed) { dirty = dirty || changed; };

				auto tag = [tagScratch = std::string{}, tagString = "##" + std::to_string(m_bufIndex)](const char* label) mutable
					{
						tagScratch = label + tagString;
						return tagScratch.c_str();
					};

				if (auto value = buf["scale"]; value.Exists())
				{
					dirtyCheck(ImGui::SliderFloat(tag("Scale"), &value, 1.0f, 2.0f, "%.3f"));
				}
				if (auto value = buf["color"]; value.Exists())
				{
					dirtyCheck(ImGui::ColorPicker4(tag("Color"), reinterpret_cast<float*>(&static_cast<DirectX::XMFLOAT4&>(value))));
				}
				if (auto v = buf["specularIntensity"]; v.Exists())
				{
					dirtyCheck(ImGui::SliderFloat(tag("Spec. Intens."), &v, 0.0f, 1.0f));
				}
				if (auto v = buf["specularPower"]; v.Exists())
				{
					dirtyCheck(ImGui::SliderFloat(tag("Glossiness"), &v, 1.0f, 100.0f, "%.1f"));
				}
				return dirty;
			}
		} probe;

		// static Probe probe;
		Accept(probe); // 显示常数缓存的UI
	}
	ImGui::End();
}

