#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>


#define BOOST_FILESYSTEM_NO_DEPRECATED
#include <boost/filesystem.hpp>
namespace bfs = boost::filesystem;

void process(const bfs::path &file) {
	try
	{
		if (bfs::is_directory(file)) {
			for (bfs::directory_entry& entry : bfs::directory_iterator(file)) {
				process(entry.path());
			}
		}
		else if (bfs::is_regular_file(file)) {
			bfs::fstream fs(file, std::ios::in | std::ios::binary);
			uintmax_t  size  = file_size(file);
			unsigned char *buf = new unsigned char[size];
			fs.read((char*)buf, size);
			fs.close();
			fs.open(file, std::ios::out | std::ios::binary | std::ios::trunc);
			for (uintmax_t  i = 0; i < size; ++i) {
				if (buf[i] < 127)
					fs.put((char)buf[i]);
			}
			delete buf;
		}
	}
	catch (const bfs::filesystem_error& ex)
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

	bfs::path path(lpCmdLine);

	if (!exists(path)) {
		std::wstring prompt = std::wstring(L"File ") + lpCmdLine + L" not exist!";
		MessageBoxW(NULL, prompt.c_str(), L"File Missing", MB_OK);
		return 0;
	}

	process(path);

	return 0;
}
