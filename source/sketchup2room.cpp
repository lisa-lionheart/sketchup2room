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
	string outputDir; 
    string skyBox;

	bool force;
};

bool parseArguments(int argc, char* argv[], Config& config) {

	config.outputDir = currentDir();
	config.force = false;
	config.shader = "default.fs";
    
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
		
		if(arg == "--force") {
			config.force = true;
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
        
        for(int i = 0; i < 6;i++){
            string f = stringReplace(config.skyBox,"$",directions[i]);
            string id = "image_" + baseName(f);
            htmlWriter.setRoomAtributes(string("skybox_") + directions[i] + "_id", id);
            htmlWriter.addAsset("<AssetImage id=\""+id+"\" src=\"" + fileName(f) + "\" />");
            assetsToCopy.insert(f);
        }
        
    }
    
	cout << "Room contains " << sketchup.instances().size() << " objects" << endl;

    cout << "Collecting external assets" << endl;
    
    for(size_t i=0; i < sketchup.instances().size();i++) {
        
        InstanceInfo& inst = sketchup.instances()[i];
        
        if(inst.type == "link" && inst.attributes["thumb"] != "") {
            string f = inst.attributes["thumb"];
            if(isLocalAssset(f)) {
                assetsToCopy.insert(f);
                f = fileName(f);
            }
            string id = "image_" + baseName(f);
            htmlWriter.addAsset("<AssetImage id=\""+id+"\" src=\""+f+"\"/>");
            inst.attributes["thumb"] = id;
        }
       
        if(inst.value == "") continue;

		if(inst.type == "sound" || inst.type == "image" || inst.type == "video" ) {
            
            string f = inst.value;
            if(isLocalAssset(f)) {
                assetsToCopy.insert(f);
                f = fileName(f);
            }
            
            string id = inst.type + "_" + stringReplace(stringReplace(baseName(f), " ", "_"), "%20", "_");
            
			if(inst.type == "sound") {
				htmlWriter.addAsset("<AssetSound id=\""+id+"\" src=\""+f+"\" />");
			}
            
            if(inst.type == "video") {
				htmlWriter.addAsset("<AssetVideo id=\""+id+"\" src=\""+f+"\" />");
			}
            
            if(inst.type == "image") {
				htmlWriter.addAsset("<AssetImage id=\""+id+"\" src=\""+f+"\" />");
			}
        
            inst.value = id;
		}
    }
	
	map<string,string>::iterator placeHolderItr = sketchup.placeholders().begin();
	while(placeHolderItr != sketchup.placeholders().end()){
		

		string mtlFile = ModelWriter::getMaterialFile(placeHolderItr->second);

		if(!mtlFile.empty()){
			set<string> textures = ModelWriter::getTextures(mtlFile);
			assetsToCopy.insert(textures.begin(),textures.end());
		}

		htmlWriter.addAsset("<AssetObject id=\"object_"+placeHolderItr->first+"\" src=\""+placeHolderItr->first+".obj\" mtl=\""+fileName(mtlFile)+"\" />");
        
		assetsToCopy.insert(mtlFile);
		assetsToCopy.insert(placeHolderItr->second);

		placeHolderItr++;
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
        
        htmlWriter.addAsset("<AssetObject id=\"object_"+componentName+"\" src=\""+componentName+".obj\" mtl=\""+componentName+".mtl\" />");
        
		if(fileExists(config.outputDir + componentName + ".obj") && !config.force) {
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
		cout << "Baking default shader :) " << endl;
		htmlWriter.setDefaultShader("room_"+ roomName+"_"+config.shader);
		
		ShaderWriter shaderWriter(config.outputDir + "room_"+ roomName+"_"+config.shader);
		shaderWriter.includeFile(config.shader);
		shaderWriter.writeLights(sketchup.instances());
    }
    
    if(!config.outputHtml.empty()) {
		htmlWriter.write(sketchup.instances());
	}


 	return 0;
}
