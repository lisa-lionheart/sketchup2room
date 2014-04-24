// sketchup2room.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "SketchupHelper.h"
#include "HtmlWriter.h"
#include "ModelWriter.h"
#include "ShaderWriter.h"

struct Config {
    string inputFile;
    string outputHtml;
    
    string ambientSound;
	string shader;
	string outputDir = currentDir();
};

bool parseArguments(int argc, char* argv[], Config& config) {
    
	for(int i=1; i < argc-1; i++){
		string arg = argv[i];
        
		if(arg == "--out"){
			config.outputDir = currentDir() + "/" + string(argv[++i]) + "/";
			makeDir(config.outputDir);
			continue;
		}
		
		if(arg == "--html") {
			config.outputHtml = argv[++i];
			continue;
		}
		
		if(arg == "--shader") {
			config.shader = argv[++i];
			continue;
		}
        
        if(arg == "--ambientSound") {
			config.ambientSound = argv[++i];
            continue;
        }
        
		cerr << "Unknown option: " << arg << endl;
		return true;
	}
    
	config.inputFile = argv[argc-1];
    
    return false;
}

int main(int argc, char* argv[])
{
	if(argc < 2){
		cerr << "sketchup2room: Converts a sketchup file into an OBj file that you can use with the JanusVR browser" << endl;
		cerr << "Arguments:  sketchup2room <options> filename" << endl << endl;
		cerr << "Options:" << endl;
		cerr << " --out <filename>          Set the output directory" << endl;
		cerr << " --html <filename>         Generate a generic firebox HTML file" << endl;
        cerr << " --ambientSound <filename> Ambient sound loop" << endl;
		return 1;
	}

	SUInitialize();
    Config config;
    if(parseArguments(argc,argv,config)) {
        return 1;
    }

	cout << "Input file: " << config.inputFile << endl;

	SUModelRef model = SketchupHelper::openFile(config.inputFile);
	if(model.ptr == 0){
		cout << "Could not read file" << endl;
		return 1;
	}
    
    
	HtmlWriter htmlWriter(config.outputDir, config.outputHtml);
	SUEntitiesRef ents = SU_INVALID;
	vector<InstanceInfo> instances;
    
	string roomName = config.inputFile.substr(0,config.inputFile.length()-4);
	
	// Get the entity container of the model.
	SUModelGetEntities(model, &ents);
	SketchupHelper::getInstancesRecursive(ents,instances);

	cout << "Room contains " << instances.size() << " objects" << endl;

	
	
	cout << "Writing world geometry" << endl;
	ModelWriter hull(config.outputDir, roomName);
	hull.write(model);

    cout << "Exporting geometry" << endl;
    
    map<string,SUComponentDefinitionRef> components = SketchupHelper::getComponents(instances);
    map<string,SUComponentDefinitionRef>::iterator itr = components.begin();
    while(itr != components.end()) {
        
        string componentName = itr->first;
        
        if(componentName[0] == '!') {
            itr++;
            continue;
		}
        
        htmlWriter.addAsset("<AssetObject id=\""+componentName+"_id\" src=\""+componentName+".obj\" mtl=\""+componentName+".mtl\" />");
        
        if(fileExists(config.outputDir + componentName + ".obj")) {
            cout << "File " << componentName << ".obj exists, skipping..." << endl;
        } else {
			
            SUComponentDefinitionGetEntities(itr->second,&ents);
            
            cout << "Writing component " << componentName << endl;
            ModelWriter writer(config.outputDir,componentName);
            writer.write(ents,true);
        }
        
        
        itr++;
        
    }
    
    if(!config.shader.empty()) {
		cout << "Baking shaders :) " << endl;
		htmlWriter.setDefaultShader("room_"+ roomName+".frag");
		
		ShaderWriter shaderWriter(config.outputDir + "room_"+ roomName+".frag", config.shader);
		shaderWriter.writeLights(instances);
    }
    
    if(!config.outputHtml.empty()) {
		htmlWriter.write(instances);
	}


	return 0;
}
