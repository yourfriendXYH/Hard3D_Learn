#pragma once

// 用于遍历Model的缓存数据，显示UI的控制面板
class ModelProbe
{
public:
	virtual bool PushNode(class Node& node) = 0;
	virtual void PopNode(class Node& node) = 0;
};