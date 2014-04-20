// sketchup2room.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "SketchupHelper.h"
#include "HtmlWriter.h"
#include "ModelWriter.h"

string currentDir() {
	char buffer[500];
	GetCurrentDirectoryA(500,buffer);
	return buffer;
}

void makeDir(string dir) {
	CreateDirectoryA(dir.c_str(),NULL);
}


int main(int argc, char* argv[])
{
	if(argc < 2){
		cerr << "sketchup2room: Converts a sketchup file into an OBj file that you can use with the JanusVR browser" << endl;
		cerr << "Arguments:  sketchup2room <options> filename" << endl << endl;
		cerr << "Options:" << endl;
		cerr << " --out <filename>       Set the output directory" << endl;
		cerr << " --html <filename>      Generate a generic firebox HTML file" << endl;
		return 1;
	}

	SUInitialize();


	string outputHtml;
	string outputDir = currentDir();

	for(int i=1; i < argc-1; i++){
		char* arg = argv[i];

		if(_stricmp("--out",arg) == 0){
			
			outputDir = currentDir() + "/" + string(argv[++i]) + "/";
			makeDir(outputDir);

		} else if(_stricmp("--html",arg) == 0) {
			outputHtml = argv[++i];
		} else {
			cerr << "Unknown option: " << arg << endl;
			return 1;
		}
	}
	
	string filename = argv[argc-1];

	SUModelRef model = SketchupHelper::openFile(filename);
	if(model.ptr == 0){
		cout << "Could not read file" << endl;
		return 1;
	}
	filename = filename.substr(0,filename.length()-4);
	


	if(outputHtml.length() != 0) {
		HtmlWriter writer(outputDir, filename);
		writer.write(model);
	}

	
	cout << "Writing world geometry" << endl;
	ModelWriter hull(outputDir, filename);
	hull.write(model);

	ModelWriter::exportComponents(model, outputDir);

	return 0;
}
