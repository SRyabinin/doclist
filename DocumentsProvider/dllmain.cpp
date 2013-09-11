// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <httpext.h>
#include <stdio.h>
#include "Index.h"
#include "Utility.h"
#include "Repository.h"

HMODULE g_hModule;
namespace Constants
{
	static const char* GET = "GET";
	const std::string repository("repository");
	const std::string RepositoryPath("Data\\");
	const std::string RepositoryExtention(".txt");
	const std::string first("first");
	const std::string last("last");

	std::string makeRepositoryPath(const std::string& name)
	{
		char buff[MAX_PATH];

		HMODULE hm = NULL;

		if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
								(LPCSTR) &makeRepositoryPath, 
									&hm))
		{
			int ret = GetLastError();
			fprintf(stderr, "GetModuleHandle returned %d\n", ret);
		}

		GetModuleFileNameA(hm, buff, MAX_PATH);
		std::string sitePath = utility::getParent(utility::getParent(utility::getParent(buff)));
		size_t pos = sitePath.find("\\\\?\\");
		if(pos != std::string::npos)
			sitePath =  sitePath.substr(4);

		return sitePath +"\\" + RepositoryPath + name+RepositoryExtention;
	}
};

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	g_hModule = hModule;
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

void WriteContext(EXTENSION_CONTROL_BLOCK *pECB, char *pszFormat, ...);

BOOL WINAPI GetExtensionVersion(HSE_VERSION_INFO *pVer)
{
	pVer->dwExtensionVersion = HSE_VERSION;
	strncpy(pVer->lpszExtensionDesc, "DocList ISAPI Extension", HSE_MAX_EXT_DLL_NAME_LEN);
	return TRUE;
}

void setHeader(EXTENSION_CONTROL_BLOCK *pECB, const std::string& status, long contextLength)
{
  HSE_SEND_HEADER_EX_INFO headerExInfo;

	std::ostringstream oss;
	oss << "Connection: Keep-Alive\r\n";
	oss << "Content-Length: " << contextLength <<"\r\n";
    oss << "Content-type: application/json; charset=windows-1251\r\n\r\n";
	std::string header = oss.str();

	headerExInfo.pszHeader = header.c_str();
	headerExInfo.cchHeader = header.size();
	headerExInfo.pszStatus = status.c_str();
    headerExInfo.cchStatus = status.size();
    headerExInfo.fKeepConn = TRUE;

    pECB->ServerSupportFunction(pECB->ConnID, HSE_REQ_SEND_RESPONSE_HEADER_EX, &headerExInfo, NULL, NULL);
}

DWORD WINAPI HttpExtensionProc(EXTENSION_CONTROL_BLOCK *pECB)
{
#ifdef _DEBUG
	char dir[MAX_PATH];
	::GetCurrentDirectoryA(MAX_PATH, (char*)dir);
#endif

	if(_stricmp(Constants::GET, pECB->lpszMethod))
		return HSE_STATUS_ERROR;

	std::string result;
	DWORD status;
	try
	{
		std::map<std::string, std::string> params = utility::parseQueryString(pECB->lpszQueryString);
		std::map<std::string, std::string>::const_iterator it;
		it = params.find(Constants::repository);
		if(it==params.end())
		{
			throw std::logic_error("Parameter \'repository\' not found.");
		}
		std::string path = Constants::makeRepositoryPath(it->second);

		it = params.find(Constants::first);
		if(it==params.end())
		{
			throw std::logic_error("Parameter \'first\' not found.");
		}
		long first = utility::convert<long>(it->second);

		it = params.find(Constants::last);
		if(it==params.end())
		{
			throw std::logic_error("Parameter \'last\' not found.");
		}
		long last = utility::convert<long>(it->second);

		Repository repository;
		result = repository.GetItems(path, first, last);
		status = HSE_STATUS_SUCCESS;
	}
	catch(const std::exception& e)
	{
		std::ostringstream oss;
		oss << "{Status:\"FAIL\", Message:\""<<e.what()<<"\"}";
		result = oss.str();
		status =  HSE_STATUS_ERROR;
	}
	//result = "[{\"id\":0, \"text\":\"\\\"Чудотворец был высокого роста\\\"\\\\n\"}]";
	DWORD length = result.length();
	setHeader(pECB, status == HSE_STATUS_SUCCESS ? "200 OK" : "202", length); 
	pECB->WriteClient(pECB->ConnID, (LPVOID)&result[0], &length, 0);
	return status;
}

//void WriteContext(EXTENSION_CONTROL_BLOCK *pECB, char *pszFormat, ...)
//{
//	char szBuffer[1024];
//	va_list arg_ptr;
//	va_start(arg_ptr, pszFormat); 
//	vsprintf(szBuffer, pszFormat, arg_ptr);
//	va_end(arg_ptr);
//	
//	DWORD dwSize = strlen(szBuffer);
//	pECB->WriteClient(pECB->ConnID, szBuffer, &dwSize, 0);
//}

void BuildIndex(const char* repository)
{
	Indexing::Index index(Constants::makeRepositoryPath(repository));
	index.RebuildIndex();
}


