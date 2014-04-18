// sketchup2room.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "SketchupHelper.h"



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

	SketchupHelper sketchup;

	string outputHtml;

	for(int i=1; i < argc-1; i++){
		char* arg = argv[i];

		if(stricmp("--out",arg) == 0){
			sketchup.setOutputDir(argv[++i]);
		} else if(stricmp("--html",arg) == 0) {
			outputHtml = argv[++i];
		} else {
			cerr << "Unknown option: " << arg << endl;
			return 1;
		}
	}
	
	string filename = argv[argc-1];


	if(!sketchup.openFile(filename)){
		cout << "Could not read file" << endl;
		return 1;
	}
	filename = filename.substr(0,filename.length()-4);
	
	sketchup.exportMaterials(filename + ".mtl");
	sketchup.exportHull(filename+".obj");

	sketchup.proccessInstances();

	if(outputHtml.length() != 0) {
		sketchup.writeHtmlFile(outputHtml,filename);
	}



	return 0;
}

