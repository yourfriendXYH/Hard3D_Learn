#pragma once
#include "Bindable.h"
#include <memory>

namespace Bind
{
	class VertexShader : public Bindable
	{
	public:
		VertexShader(Graphics& gfx, const std::string& path);
		void Bind(Graphics& gfx) noexcept override;
		ID3DBlob* GetByteCode() const noexcept;

		static std::shared_ptr<VertexShader> Resolve(Graphics& gfx, const std::string& path);
		static std::string GenerateUID(const std::string& path);
		std::string GetUID() const noexcept;

	private:
		std::string m_path;
		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_pVertexShader;
		// cso的字节码文件
		Microsoft::WRL::ComPtr<ID3DBlob> m_pBlob;
	};
}

