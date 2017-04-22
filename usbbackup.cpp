#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <algorithm>
#include <windows.h>
#include <io.h>
#include <ctime>
#include <sys/stat.h>
#define LOG "mybackup.log"
using namespace std;

void fileCopy(const char* src, const char* dst) ;

int isFileOrDir(const char* s);
using namespace std;
ofstream log (LOG, ios::ate);

void Log(const char* str1, const char* str2){
    time_t t = time(0);
    char *noew = ctime(&t);
  //  ofstream fout (LOG, ios::ate);
    log << noew <<" : " << str1 <<str2 << endl;
}

void Log(const char* str){
    time_t t = time(0);
    char *noew = ctime(&t);
  //  ofstream fout (LOG, ios::ate);
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

_finddata_t findFile(const char* path, const char* filename, int& x) {
	_finddata_t fd;
	long handle;
	string fullname = (string)path + "\\" + (string)filename;
	handle = _findfirst(fullname.c_str(), &fd);

	if (handle == -1) x = -1;
	else x = 0;

	return fd;
}

void allFileCopy(const char* srcpath, const char* dstpath)
{
	_finddata_t fd, fd2;
	long handle;
	int result = 1;
	int a = 1;
	string srcfile, srcfull, dstfull;
	srcfile = (string)srcpath + "\\*.*"; // find all files in srcpath
	handle = _findfirst(srcfile.c_str(), &fd);

	if (handle == -1)
	{
		cout << "There were no files in" <<srcfile <<endl;
		Log("There were no files.\n");
		return;
	}

	while (result != -1)
	{
		srcfull = (string)srcpath + "\\" + fd.name;
		dstfull = (string)dstpath + "\\" + fd.name;
		if (!strcmp(fd.name, ".") || !strcmp(fd.name, "..")) { ; }
		else {
			fd2 = findFile(dstpath, fd.name, a);
			if(isFileOrDir(srcfull.c_str())==1 ){ // it's file.
                if (a == -1) {
                    fileCopy(srcfull.c_str(), dstfull.c_str());
                    cout << fd.name << endl;
                    Log(fd.name, "is made.");

                }
                else if (fd2.time_write >= fd.time_write) {
                        cout << fd.name << "is existed" << endl;
                        Log(fd.name, "is existed.");
                }
                else {
                    fileCopy(srcfull.c_str(), dstfull.c_str());
                    cout << fd.name << "is changed" << endl;
                    Log(fd.name, "is changed.");
                }
			}
			else if (isFileOrDir(srcfull.c_str())==0){ // it's folder
                cout<<fd.name <<"is a folder."<< endl;
                Log(fd.name, "is a folder.");
                allFileCopy((srcfull +"\\"+ fd2.name).c_str(), (dstfull +"\\"+fd2.name).c_str());
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
