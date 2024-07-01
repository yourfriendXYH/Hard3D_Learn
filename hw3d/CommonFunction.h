#pragma once

#include <Windows.h>
#include <string>

class CommonFun
{
public:
	static LPCWSTR stringToLPCWSTR(std::string orig);
};