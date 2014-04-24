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

string currentDir() {
    char buff[400];
    getcwd(buff, 400);
    return buff;
}

void makeDir(string dir) {
    mkdir(dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

bool fileExists(const string& str) {
    return (access( str.c_str(), F_OK ) != -1);
}


