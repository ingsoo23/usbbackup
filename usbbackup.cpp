#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <algorithm>
#include <windows.h>
#include <io.h>
#include <sys/stat.h>
using namespace std;

bool fileExists(const char* filename) {
	struct stat buf;
	if (stat(filename, &buf) != -1) return true;
	return false;
}

int fileCopy(const char* src, const char* dst) {
	ifstream fin(src, ios::binary);
	ofstream fout(dst, ios::binary);

	copy(
		istreambuf_iterator<char>(fin),
		istreambuf_iterator<char>(),
		ostreambuf_iterator<char>(fout));

	return 1;
}

void allFileCopy(const char* srcpath, const char* dstpath)
{
	_finddata_t fd;
	long handle;
	int result = 1;
	int x;
	string srcfile, srcfull, dstfull;
	srcfile = (string)srcpath + "\\*.*"; // find all files in srcpath
	handle = _findfirst(srcfile.c_str(), &fd);

	if (handle == -1)
	{
		printf("There were no files.\n");
		return;
	}

	while (result != -1)
	{
		srcfull = (string)srcpath + "\\" + fd.name;
		dstfull = (string)dstpath + "\\" + fd.name;
		if (!fileExists(dstfull.c_str())) {
			x = fileCopy(srcfull.c_str(), dstfull.c_str());
			cout << fd.name << endl;
		}
		else cout << fd.name << " 이미 존재" << endl;
		srcfull.clear();
		dstfull.clear();
		result = _findnext(handle, &fd);
	}

	_findclose(handle);
	return;
}

int isFileOrDir(char* s) {
	_finddata_t c_file;
	intptr_t hFile;
	int result;

	if ((hFile = _findfirst(s, &c_file)) == -1L)
		result = -1; // 파일 또는 디렉토리가 없으면 -1 반환
	else if (c_file.attrib & _A_SUBDIR)
		result = 0; // 디렉토리면 0 반환
	else
		result = 1; // 그밖의 경우는 "존재하는 파일"이기에 1 반환

	_findclose(hFile);

	return result;
}

int main(int argc, char** argv) {
	if (argc!= 3){
		cerr << "ERROR" << endl;
		return -1;
	}
	string srcpath, dstpath;

	srcpath = argv[1];
	dstpath = argv[2];

    allFileCopy(srcpath.c_str(), dstpath.c_str());

	return 0;
}
