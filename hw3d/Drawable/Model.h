#pragma once
#include <memory>
#include <vector>
#include <filesystem>
#include "../Graphics.h"

class Node;
class Mesh;
class FrameCommander;

struct aiMesh;
struct aiMaterial;
struct aiNode;

class Model
{
public:
	// pathString: ģ���ļ���·��
	Model(Graphics& gfx, const std::string& pathString, float scale = 1.0f);
	~Model() noexcept;

	// �ύҪ���ƵĹ�������
	void Submit(FrameCommander& frame) const noexcept;
	// ��ʾUI����
	void ShowWindow(Graphics& gfx, const char* windowName = nullptr) noexcept;
	// ����ģ�͸��ڵ��transform
	void SetRootTransform(DirectX::FXMMATRIX transform);

private:
	// ��������ģ��
	static std::unique_ptr<Mesh> ParseMesh(Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials, const std::filesystem::path& path, float scale);
	// ����ģ�ͽڵ�
	std::unique_ptr<Node> ParseNode(int& nextId, const aiNode& node) noexcept;

private:
	std::unique_ptr<Node> m_pRoot; // ģ�͵ĸ��ڵ�
	std::vector<std::unique_ptr<Mesh>> m_meshPtrs; // ���ڵ���ص�ģ��drawable
	std::unique_ptr<class ModelWindow> m_pWindow; // ����ģ�͵�UI����
};