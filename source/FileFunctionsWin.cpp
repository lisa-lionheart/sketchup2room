//
//  FileFunctionsWin.cpp
//  sketchup2room
//
//  Created by Lisa Croxford on 22/04/2014.
//  Copyright (c) 2014 Lisa Croxford. All rights reserved.
//

#include "stdafx.h"


string currentDir() {
	char buffer[500];
	GetCurrentDirectoryA(500,buffer);
	return buffer;
}

void makeDir(string dir) {
	CreateDirectoryA(dir.c_str(),NULL);
}


bool fileExists(const string& file)
{
    WIN32_FIND_DATAA FindFileData;
    HANDLE handle = FindFirstFileA(file.c_str(), &FindFileData) ;
    int found = handle != INVALID_HANDLE_VALUE;
    if(found)
    {
        FindClose(handle);
        return true;
    }
    return false;
}
bool fileCopy(const string& src, const string& dest) {
    
    ifstream srcFile(src,ios_base::binary);
    ofstream destFile(dest, ios_base::binary);
    
    char buffer[1024];
    
    while(!srcFile.eof()){
        srcFile.read(buffer, 1024);
        destFile.write(buffer,1024);
    }
    
    return true;
}


string executablePath() {

	char buff[200];
	GetModuleFileNameA(0,buff,200);
	return buff;
}