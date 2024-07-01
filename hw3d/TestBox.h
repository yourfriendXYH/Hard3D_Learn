//#pragma once
//
//#include "Drawable.h"
//#include "Drawable/TestObject.h"
//#include <random>
//
//class TestBox : public TestObject<TestBox>
//{
//public:
//	//                                        统一的正态分布
//	TestBox(Graphics& gfx, std::mt19937& rng,
//		std::uniform_real_distribution<float>& adist,
//		std::uniform_real_distribution<float>& ddist,
//		std::uniform_real_distribution<float>& odist,
//		std::uniform_real_distribution<float>& rdist,
//		std::uniform_real_distribution<float>& bdist,
//		DirectX::XMFLOAT3 material);
//
//	DirectX::XMMATRIX GetTransformXM() const noexcept override;
//
//private:
//
//	// model transform
//	DirectX::XMFLOAT3X3 m_mt;
//
//};