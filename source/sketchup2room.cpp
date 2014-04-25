// sketchup2room.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "SketchupHelper.h"
#include "HtmlWriter.h"
#include "ModelWriter.h"
#include "ShaderWriter.h"


string stringReplace(string in, string find, string replace) {
    
    while(true) {
        size_t pos = in.find_first_of(find);
        if(pos == -1)
            break;
        
        in = in.substr(0,pos) + replace + in.substr(pos+find.length());
    }
    return in;
}

struct Config {
    string inputFile;
    string outputHtml;
    
    string ambientSound;
	string shader;
	string outputDir; 
    string skyBox;
};

bool parseArguments(int argc, char* argv[], Config& config) {

	config.outputDir = currentDir();
    
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
        
        if(arg == "--skyBox") {
			config.skyBox = argv[++i];
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
		return 1;
	}

	SUInitialize();
    Config config;
    if(parseArguments(argc,argv,config)) {
        return 1;
    }
    
    SketchupHelper sketchup;
    HtmlWriter htmlWriter(config.outputDir, config.outputHtml);
	string roomName = config.inputFile.substr(0,config.inputFile.length()-4);
    
	cout << "Input file: " << config.inputFile << endl;

	if(!sketchup.openFile(config.inputFile)){
		cout << "Could not read file" << endl;
		return 1;
	}
    
    
    set<string> assetsToCopy;
    
    if(!config.skyBox.empty()) {
        
        const char* directions[6] = {"up", "down", "left", "right", "front", "back"};
        
        for(int i = 0; i < 5;i++){
            string f = stringReplace(config.skyBox,"$",directions[i]);
            string id = "image_" + baseName(f) + "_id";
            htmlWriter.setRoomAtributes(string("skybox_") + directions[i] + "_id", id);
            htmlWriter.addAsset("<AssetImage id=\""+id+"\" src=\"" + fileName(f) + "\" />");
            assetsToCopy.insert(f);
        }
        
    }
    
	cout << "Room contains " << sketchup.instances().size() << " objects" << endl;

    cout << "Collecting external assets" << endl;
    
    for(int i=0; i < sketchup.instances().size();i++) {
        
        InstanceInfo& inst = sketchup.instances()[i];
        if(inst.value == "") continue;
        
		if(inst.type == "sound" || inst.type == "image" ) {
			if(inst.type == "sound") {
				htmlWriter.addAsset("<AssetSound id=\"sound_"+baseName(inst.value)+"_id\" src=\""+fileName(inst.value)+"\" />");
			}
        
			if(inst.value.substr(0,5) == "http:")
				continue;
        
			assetsToCopy.insert(inst.value);
		}
    }
	
    
    
    set<string>::iterator fileItr = assetsToCopy.begin();
    while(fileItr != assetsToCopy.end()){
        if(!fileExists(*fileItr)) {
            cout << "Error file " << *fileItr << " could not be located";
            return 1;
        }
        
        string dest = config.outputDir+fileName(*fileItr);
        if(fileExists(dest)) {
            cout << "Skipping " << *fileItr << " already exists in output directory" << endl;
			fileItr++;
            continue;
        }
        
        cout << *fileItr << " -> " << dest << endl;
        fileCopy(*fileItr, dest);
        
        if(fileExtension(*fileItr) == "obj") {
            
            //TODO: Copy materials and textures for object files
        }

        
        fileItr++;
    }

    
	cout << "Writing world geometry" << endl;
	ModelWriter hull(config.outputDir, roomName);
	hull.write(sketchup.topLevelEntities());

    cout << "Exporting geometry" << endl;
    map<string,SUComponentDefinitionRef>::iterator itr = sketchup.components().begin();
    while(itr != sketchup.components().end()) {
        
        string componentName = itr->first;
        
        if(componentName[0] == '!') {
            itr++;
            continue;
		}
        
        htmlWriter.addAsset("<AssetObject id=\""+componentName+"_id\" src=\""+componentName+".obj\" mtl=\""+componentName+".mtl\" />");
        
        if(fileExists(config.outputDir + componentName + ".obj")) {
            cout << "File " << componentName << ".obj exists, skipping..." << endl;
        } else {
			SUEntitiesRef ents = SU_INVALID;
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
		shaderWriter.writeLights(sketchup.instances());
    }
    
    if(!config.outputHtml.empty()) {
		htmlWriter.write(sketchup.instances());
	}


	return 0;
}
