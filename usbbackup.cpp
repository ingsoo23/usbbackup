#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <algorithm>
#include <windows.h>
#include <io.h>
#include <ctime>
#include <sys/stat.h>
#include <direct.h>
#define LOG "mybackup.log"
using namespace std;

void fileCopy(const char* src, const char* dst) ;

int isFileOrDir(const char* s);
using namespace std;
ofstream log (LOG, ios::ate);

void Log(const char* str1, const char* str2){
    time_t t = time(0);
    char *noew = ctime(&t);
    log << noew <<" : " << str1 <<str2 << endl;
}

void Log(const char* str){
    time_t t = time(0);
    char *noew = ctime(&t);
    log << noew <<" : " << str << endl;
}

void fileCopy(const char* src, const char* dst) {
	ifstream fin(src, ios::binary);
	ofstream fout(dst, ios::binary);

	copy(
		istreambuf_iterator<char>(fin),
		istreambuf_iterator<char>(),
		ostreambuf_iterator<char>(fout));
}

_finddata_t findFile(const char* path, const char* filename, int& x) { //find destination file
	_finddata_t fd;
	long handle;
	string fullname = (string)path + "\\" + (string)filename;
	handle = _findfirst(fullname.c_str(), &fd);

	if (handle == -1) x = -1; //destination file is not existed.
	else x = 0;

	return fd;
}

bool dirExists(const char* dirName_in) // which is directory or not
{
  DWORD ftyp = GetFileAttributesA(dirName_in);
  if (ftyp == INVALID_FILE_ATTRIBUTES)
    return false; //error

  if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
    return true; // it's a directory

  return false; // it's not a directory
}

void allFileCopy(const char* srcpath, const char* dstpath)
{
	_finddata_t fd, fd2;
	long handle;
	int result = 1;
	int a = 1;
	string srcfile, srcfull, dstfull;
	srcfile = (string)srcpath + "\\*.*"; // find all files in srcpath

	if(!dirExists(dstpath)){ // if dstpath is not existed, make directory.
        mkdir(dstpath);
	}

	handle = _findfirst(srcfile.c_str(), &fd);

	if (handle == -1)
	{
		Log("There were no files.\n");
		return;
	}

	while (result != -1)
	{
		srcfull = (string)srcpath + "\\" + fd.name;
		dstfull = (string)dstpath + "\\" + fd.name;

		if (!strcmp(fd.name, ".") || !strcmp(fd.name, "..")) { ; } // except ".", ".."

		else {
			fd2 = findFile(dstpath, fd.name, a); // find file fd.name in dstpath

			if(isFileOrDir(srcfull.c_str())==1 ){ // when src is a file.

                if (a == -1) { //fd.name is not existed in dstpath
                    fileCopy(srcfull.c_str(), dstfull.c_str());
                    Log(fd.name, " is made.");
                }

                else if (fd2.time_write >= fd.time_write) { // if dst changed after src changed
                        Log(fd.name, " is existed.");
                }

                else { //backup file
                    fileCopy(srcfull.c_str(), dstfull.c_str());
                    Log(fd.name, " is changed.");
                }
			}

			else if (isFileOrDir(srcfull.c_str())==0){ // when src is a folder.
                Log(fd.name, " is a folder.");
                mkdir((dstfull).c_str()); //make a folder in dst
                allFileCopy((srcfull).c_str(), (dstfull).c_str()); //recursive
			}

			else{// error
                Log("ERROR");
			}
		}

		srcfull.clear();
		dstfull.clear();
		result = _findnext(handle, &fd);
	}

	_findclose(handle);
	return;
}

int isFileOrDir(const char* s) {
	_finddata_t c_file;
	intptr_t hFile;
	int result;

	if ((hFile = _findfirst(s, &c_file)) == -1L)
		result = -1; // if no file or directory

	else if (c_file.attrib & _A_SUBDIR)
		result = 0; // if it's a directory

	else
		result = 1; // if it's a file

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
