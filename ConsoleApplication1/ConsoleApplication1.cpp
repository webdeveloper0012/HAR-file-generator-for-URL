// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <Windows.h>
#include "resource.h"

#define _QUOTE(X) #X
#define QUOTE(X) _QUOTE(X)
#define RES_TYPE QUOTE(RES_TYPE_UNQUOTED)
#define RES_NAME QUOTE(RES_NAME_UNQUOTED)

#include <filesystem>

#include <Windows.h>
#include <WinDef.h>
#include <Shellapi.h>
#include <iostream>
#include <string>
#include <ShlDisp.h>
#include <Shlobj.h>

#include "unzip.h"
#include <atlstr.h>



using namespace std;

std::wstring s2ws(const std::string& s)
{

	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

size_t wstrlcat(wchar_t *dst, const wchar_t *src, size_t siz)
{
	wchar_t *d = dst;
	const wchar_t *s = src;
	size_t n = siz;
	size_t dlen;

	while (n-- != 0 && *d != L'\0') {
		d++;
	}

	dlen = d - dst;
	n = siz - dlen;

	if (n == 0) {
		return(dlen + wcslen(s));
	}

	while (*s != L'\0')
	{
		if (n != 1)
		{
			*d++ = *s;
			n--;
		}
		s++;
	}

	*d = '\0';
	return(dlen + (s - src));        
}

BSTR ConvertMBSToBSTR(const std::string& str)
{
	int wslen = ::MultiByteToWideChar(CP_ACP, 0,
		str.data(), str.length(),
		NULL, 0);

	BSTR wsdata = ::SysAllocStringLen(NULL, wslen);
	::MultiByteToWideChar(CP_ACP, 0,
		str.data(), str.length(),
		wsdata, wslen);
	return wsdata;
}

int RunResource(LPCWSTR dir) {
	HANDLE fp = NULL;
	HRSRC res = NULL;
	HGLOBAL global = NULL;
	char *buf = NULL;
	DWORD size = 0, written = 0;
	int ret = -1;

	wchar_t filestr[1024];
	wcscpy_s(filestr, dir);
	wstrlcat(filestr, L"\\file.zip", 1024);


	if ((res = FindResource(NULL, TEXT(RES_NAME), TEXT(RES_TYPE))) == NULL) {

		return 1;
	}
	if ((global = LoadResource(NULL, res)) == NULL) {

		return 2;
	}
	if ((size = SizeofResource(NULL, res)) == 0) {

		return 3;
	}
	if ((buf = (char*)LockResource(global)) == NULL) {

		return 4;
	}
	if ((fp = CreateFileW(filestr, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE) {

		return 5;
	}
	
		DWORD at;
		if (WriteFile(fp, buf, size, &at, NULL) == FALSE) {

			CloseHandle(fp);
			return 5;
		}
	
	
	
	CloseHandle(fp);

	

	TCHAR f[MAX_PATH] = { 0 };
	
	_tcscpy_s(f, MAX_PATH, filestr);

	TCHAR cdir[MAX_PATH] = { 0 };
	DWORD dwRet;
	dwRet = GetCurrentDirectory(MAX_PATH, cdir);
	SetCurrentDirectory(dir);

	HZIP hz = OpenZip(f, 0);
	ZIPENTRY ze; GetZipItem(hz, -1, &ze); int numitems = ze.index;
	
	

	for (int zi = 0; zi<numitems; zi++)
	{

		ZIPENTRY ze; GetZipItem(hz, zi, &ze); 
		UnzipItem(hz, zi, ze.name);         
	}
	CloseZip(hz);

	SetCurrentDirectory(cdir);

	return 0;
}


void ShowOptions()
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, 2);
	cout << "\n Usage 1: HAR.exe <browser_name> <url>";   
	cout << "\n Usage 2: HAR.exe <browser_name> <url_list>";
	cout << "\n Usage 3: HAR.exe <browser_name> <url_file>";
	cout << "\n Usage 4: HAR.exe <browser_name> \"<options>\"";
	cout << "\n\n <browser_name> ";
	SetConsoleTextAttribute(hConsole, 15);
	cout << "this is the mandatory keyword which may be any reserved name registered by the installed Web Browser\n\t\t which takes command line arguments and supports Remote Debugging Protocol.\n";
	cout <<	"\n\t Examples:\n\t\tHAR.exe chrome http://url.com\n\t\tHAR.exe opera http://url.com\n\t\tHAR.exe firefox http://url.com\n\t\tHAR.exe msedge http://url.com\n\t\tHAR.exe edge http://url.com\n\t\tHAR.exe safari http://url.com";
	SetConsoleTextAttribute(hConsole, 2);
	cout << "\n\n <url> ";
	SetConsoleTextAttribute(hConsole, 15);
	cout << "this is the page web address, which should be written correctly, indicating the HTTP or HTTPS protocol: \n\n";
	cout << "\n\t Examples: HAR.exe chrome http://website.com";
	SetConsoleTextAttribute(hConsole, 2);
	cout << "\n\n <url_list> ";
	SetConsoleTextAttribute(hConsole, 15);
	cout << "this allows to process a list of URLs, using left angle bracket, double quotes and single space between URLs";
	cout << "\n\tExamples: \n\t\t\HAR.exe chrome \"< http://url_1.com http://url_2.com\"";
	cout << "\n\t\tHAR.exe chrome \"< \"\"http://url_1.com/page 1/index.html\"\" \"\"http://url_2.com/how to/use\"\"\"";
	SetConsoleTextAttribute(hConsole, 2);
	cout << "\n\n <url_file> ";
	SetConsoleTextAttribute(hConsole, 15);
	cout << "this is the file path with file name of a textual file which contains multiple URLs, written one url per row";
	cout << "\n\tExamples: \n\t\tHAR.exe chrome \"< \"C:\\documents\\file_with_urls_one_per_line.txt\"";
	SetConsoleTextAttribute(hConsole, 2);
	cout << "\n\n <options> ";
	SetConsoleTextAttribute(hConsole, 15);
	cout << "this are options that may be used to generate HAR files. \n\tThe following options are available : \n\n";
	cout << "\t\t--width <dip>        frame width in DIP\n\
		--height <dip>       frame height in DIP\n\
		--output <file>      write to file instead of stdout\n\
		--content            also capture the requests body\n\
		---cache              allow caching\n\
		--agent <agent>      user agent override\n\
		---block <URL>        URL pattern(*) to block(can be repeated)\n\
		---header <header>    Additional headers(can be repeated)\n\
		--insecure           ignore certificate errors\n\
		--grace <ms>         time to wait after the load event\n\
		--timeout <ms>       time to wait before giving up with a URL\n\
		--retry <number>     number of retries on page load failure\n\
		--retry - delay <ms>   time to wait before starting a new attempt\n\
		--abort - on - failure   stop after the first failure(incompatible with parallel mode)\n\
		--post - data <bytes>  maximum POST data size to be returned\n\
		--parallel <n>       load <n> URLs in parallel\n";
	
	cout << "\n\tExamples:  HAR.exe chrome \"http://url.com --output myhar.txt\"\n\n";
	
}



int main(int argc, char *argv[])
{

	if (argc == 1 || argc < 3) { ShowOptions(); system("pause"); return 1; }

	TCHAR szPath[MAX_PATH] = { 0 };
	wchar_t td[1024];
	GetTempPath(1024, td);

	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, td)))
	{
		PathAppend(td, _T("\\getHar"));
	}
	std::wstring szp(&td[0]);
	DWORD ftyp = GetFileAttributesW(szp.c_str());
	if (ftyp == 0xffffffff )
	{
		CreateDirectory(td, NULL);
	}

	int i = RunResource(td); 

	std::wstring wapp = s2ws(string(argv[1]));
	LPCWSTR app = wapp.c_str();


	SHELLEXECUTEINFO shellExInfo1;
	shellExInfo1.cbSize = sizeof(SHELLEXECUTEINFO);
	shellExInfo1.fMask = SEE_MASK_NOCLOSEPROCESS;
	shellExInfo1.hwnd = NULL;
	shellExInfo1.lpVerb = L"open";
	shellExInfo1.lpFile = app;
	shellExInfo1.lpParameters = L"--remote-debugging-port=9222 --headless --disable-gpu";
	shellExInfo1.lpDirectory = NULL;
	shellExInfo1.nShow = SW_HIDE;
	shellExInfo1.hInstApp = NULL;
	ShellExecuteEx(&shellExInfo1); 

	Sleep(1000);


	wchar_t node[1024];
	wcscpy_s(node, td);
	wstrlcat(node, L"\\node.exe", 1024);

	wchar_t param[1024];
	wcscpy_s(param, L"\"");
	wstrlcat(param, td, 1024);
	wstrlcat(param, L"\\node_modules\\chrome-har-capturer\\bin\\cli.js\" -c -o result.har ", 1024);
	wstrlcat(param, s2ws(string(argv[2])).c_str(), 1024);


	SHELLEXECUTEINFO shellExInfo2;
	shellExInfo2.cbSize = sizeof(SHELLEXECUTEINFO);
	shellExInfo2.fMask = SEE_MASK_NOCLOSEPROCESS;
	shellExInfo2.hwnd = NULL;
	shellExInfo2.lpVerb = L"open";
	shellExInfo2.lpFile = node;
	shellExInfo2.lpParameters = param;
	shellExInfo2.lpDirectory = NULL;
	shellExInfo2.nShow = SW_HIDE;
	shellExInfo2.hInstApp = NULL;
	ShellExecuteEx(&shellExInfo2); 

	WaitForSingleObject(shellExInfo2.hProcess, INFINITE);
	CloseHandle(shellExInfo2.hProcess);

	TerminateProcess(shellExInfo1.hProcess, 1);
	TerminateProcess(shellExInfo2.hProcess, 1);

    return 0;
}

