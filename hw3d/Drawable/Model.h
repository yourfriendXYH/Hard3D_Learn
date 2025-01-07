#pragma once
#include <memory>
#include <vector>
#include <filesystem>
#include "../Graphics.h"

class Node;
class Mesh;
class FrameCommander;
class ModelWindow;

struct aiMesh;
struct aiMaterial;
struct aiNode;

class Model
{
public:
	// pathString: ģ���ļ���·����scale: ģ�ʹ�С�����ű�����
	Model(Graphics& gfx, const std::string& pathString, float scale = 1.0f);
	~Model() noexcept;

	// �ύҪ���ƵĹ�������
	void Submit(FrameCommander& frame) const noexcept;
	// ��ʾUI����
	void ShowWindow(Graphics& gfx, const char* windowName = nullptr) noexcept;
	// ����ģ�͸��ڵ��transform
	void SetRootTransform(DirectX::FXMMATRIX transform);

private:
	// ����ģ�ͽڵ�
	std::unique_ptr<Node> ParseNode(int& nextId, const aiNode& nodem, DirectX::FXMMATRIX additionalTransform) noexcept;

private:
	std::unique_ptr<Node> m_pRoot; // ģ�͵ĸ��ڵ�
	std::vector<std::unique_ptr<Mesh>> m_meshPtrs; // ���ڵ���ص�ģ��drawable
	std::unique_ptr<ModelWindow> m_pWindow; // ����ģ�͵�UI����
};