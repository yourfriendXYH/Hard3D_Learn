#pragma once
#include "Bindable.h"
#include <memory>

namespace Bind
{
	class Texture : public Bindable
	{
	public:
		Texture(Graphics& gfx, const std::string& path, UINT slot = 0u);
		void Bind(Graphics& gfx) noexcept override;

		static std::shared_ptr<Texture> Resolve(Graphics& gfx, const std::string& path, UINT slot = 0u);
		static std::string GenerateUID(const std::string& path, UINT slot = 0u);
		std::string GetUID() const noexcept;

		bool HasAlpha() const noexcept;
	protected:

		bool m_hasAlpha = false;
		std::string m_path; // ����·��
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTextureView;
	private:
		UINT m_slot; // ��������ɫ���ϵĲ��
	};
}

