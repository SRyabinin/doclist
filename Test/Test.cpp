// Test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <exception>
#include <sstream>
typedef void (* TBuildIndex)(const char*);

int _tmain(int argc, _TCHAR* argv[])
{
	char dir[MAX_PATH];
	::GetCurrentDirectoryA(MAX_PATH, (char*)dir);

	try
	{
		HMODULE hMod = ::LoadLibrary(_T("isapi\\Debug\\DocumentsProvider.dll"));
		if (!hMod) 
		{
			hMod = ::LoadLibrary(_T("isapi\\Release\\DocumentsProvider.dll"));
		}
		if (hMod) 
		{
			TBuildIndex buildIndex = (TBuildIndex)::GetProcAddress(hMod,"BuildIndex");
			if (buildIndex)
			{
				buildIndex("DocList");
			}
			FreeLibrary(hMod);
		}
	}
	catch(const std::exception& e)
	{
		std::stringstream ss;
		ss << "Exception: "<< e.what() << std::endl;
		OutputDebugStringA(ss.str().c_str());
	}

	return 0;
}

