//
//  FileFunctionsMac.cpp
//  sketchup2room
//
//  Created by Lisa Croxford on 22/04/2014.
//  Copyright (c) 2014 Lisa Croxford. All rights reserved.
//

#include "stdafx.h"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/kernel.h>

#include <mach-o/dyld.h>


string currentDir() {
    char buff[400];
    getcwd(buff, 400);
    return buff;
}

string executablePath() {
    char path[1024];
    uint32_t size = sizeof(path);
    _NSGetExecutablePath(path, &size);
    return path;
}

void makeDir(string dir) {
    mkdir(dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

bool fileExists(const string& str) {
    return (access( str.c_str(), F_OK ) != -1);
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

