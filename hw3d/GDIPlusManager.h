#pragma once
#include "XYHWin.h"

// �����͹ر�GDI+

class GDIPlusManager
{
public:
	GDIPlusManager();
	~GDIPlusManager();
private:
	static ULONG_PTR token;
	static int refCount;
};