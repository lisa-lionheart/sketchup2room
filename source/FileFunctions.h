//
//  FileFunctions.h
//  sketchup2room
//
//  Created by Lisa Croxford on 22/04/2014.
//  Copyright (c) 2014 Lisa Croxford. All rights reserved.
//

#pragma once


string currentDir();
void makeDir(string dir);
bool fileExists(const string& file);
bool fileCopy(const string& src, const string& dest);

string executablePath();

inline string sdkDir() {
	string exe = executablePath();
	return stringReplace(stringReplace(exe,"\\bin\\sketchup2room.exe",""),"/bin/sketchup2room","");
}

inline string fileName(const string& path) {
    int i = path.find_last_of('/');
    return (i==-1) ? path : path.substr(i+1);
}

inline string dirName(const string& path) {
    int i = path.find_last_of('/');
    return (i==-1) ? path : path.substr(0,i+1);
}

inline string baseName(const string& path) {
    int i = path.find_last_of('/');
    int j = path.find_last_of('.');
    return (i==-1) ? path.substr(0,j) : path.substr(i+1,j-i-1);
}


inline string fileExtension(const string& path) {
    int i = path.find_last_of('.');
    return (i==-1) ? baseName(path) : path.substr(i+1);
}

inline bool isLocalAssset(const string& url) {
    
    if(url.substr(0,4) == "http") {
        return false;
    }
    
    return true;
}