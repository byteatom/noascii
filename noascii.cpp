#include "stdafx.h"
#include "noascii.h"

#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
using namespace boost::filesystem;
using namespace std;

void process(path &file) {
	try
	{
		if (is_directory(file)) {
			for (directory_entry& entry : directory_iterator(file)) {
				path subFile = entry.path();
				process(subFile);
			}
		}
		else if (is_regular_file(file)) {
			boost::filesystem::fstream fs(file, ios::in | ios::binary);	
			uintmax_t  size  = file_size(file);			
			unsigned char *buf = new unsigned char[size];			
			fs.read((char*)buf, size);
			fs.close();
			fs.open(file, ios::out | ios::binary | ios::trunc);			
			for (uintmax_t  i = 0; i < size; ++i) {
				if (buf[i] < 127)
					fs.put((char)buf[i]);
			}
			delete buf;
		}
	}
	catch (const filesystem_error& ex)
	{
		MessageBoxA(NULL, ex.what(), "Exception", MB_OK);
	}
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	if (nullptr == lpCmdLine || 0 == wcslen(lpCmdLine)) {
		MessageBoxW(NULL, L"Usage: noascii.exe file_or_dir", L"File Missing", MB_OK);
		return 0;
	}

	path path(lpCmdLine);

	if (!exists(path)) {
		std::wstring prompt = std::wstring(L"File ") + lpCmdLine + L" not exist!";
		MessageBoxW(NULL, prompt.c_str(), L"File Missing", MB_OK);
		return 0;
	}

	process(path);

	return 0;
}
