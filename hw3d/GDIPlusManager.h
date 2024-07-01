#pragma once
#include "XYHWin.h"

// Æô¶¯ºÍ¹Ø±ÕGDI+

class GDIPlusManager
{
public:
	GDIPlusManager();
	~GDIPlusManager();
private:
	static ULONG_PTR token;
	static int refCount;
};