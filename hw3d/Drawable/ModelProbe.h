#pragma once

// ���ڱ���Model�Ļ������ݣ���ʾUI�Ŀ������
class ModelProbe
{
public:
	virtual bool PushNode(class Node& node) = 0;
	virtual void PopNode(class Node& node) = 0;
};