// sketchup2room.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Sketchup2Room.h"

SketchUp2Room::SketchUp2Room() : m_HtmlWriter(m_Config) {
	SUInitialize();
}

bool SketchUp2Room::parseArguments(int argc, char* argv[]) {

	if(argc < 2){
		cerr << "sketchup2room: Converts a sketchup file into an OBj file that you can use with the JanusVR browser" << endl;
		cerr << "Arguments:  sketchup2room <options> filename" << endl << endl;
		cerr << "Options:" << endl;
		cerr << " --out <filename>          Set the output directory" << endl;
		cerr << " --html <filename>         Generate a generic firebox HTML file" << endl;
		return false;
	}

	m_Config.outputDir = currentDir();
	m_Config.force = false;
	m_Config.shader = "default.fs";
	m_Config.templateName = "default.templ";

	for(int i=1; i < argc-1; i++){
		string arg = argv[i];
        
		if(arg == "--out"){
			m_Config.outputDir = currentDir() + "/" + string(argv[++i]) + "/";
			makeDir(m_Config.outputDir);
			continue;
		}
		
		if(arg == "--html") {
			m_Config.outputHtml = argv[++i];
			continue;
		}
		
		if(arg == "--shader") {
			m_Config.shader = argv[++i];
			continue;
		}
        
        if(arg == "--skyBox") {
			m_Config.skyBox = argv[++i];
			continue;
		}
		
		if(arg == "--force") {
			m_Config.force = true;
			continue;
		}
		cerr << "Unknown option: " << arg << endl;
		return true;
	}
    
	m_Config.inputFile = argv[argc-1];

    m_Config.roomName = m_Config.inputFile.substr(0,m_Config.inputFile.length()-4);

    if(!m_Config.skyBox.empty()) {
        
        const char* directions[6] = {"up", "down", "left", "right", "front", "back"};
        
        for(int i = 0; i < 6;i++){
            string f = stringReplace(m_Config.skyBox,"$",directions[i]);
            string id = "image_" + baseName(f);
            m_HtmlWriter.setRoomAtributes(string("skybox_") + directions[i] + "_id", id);
            m_HtmlWriter.addAsset("<AssetImage id=\""+id+"\" src=\"" + fileName(f) + "\" />");
            m_AssetsToCopy.insert(f);
        }
        
    }
    return false;
}


void SketchUp2Room::readFile() {
    
	cout << "Input file: " << m_Config.inputFile << endl;

	if(!m_Helper.openFile(m_Config.inputFile)){
		cout << "Could not read file" << endl;
		exit(1);
	}
    
	
	cout << "Room contains " << m_Helper.instances().size() << " objects" << endl;
}

void SketchUp2Room::saveExternals() {
	 cout << "Collecting external assets" << endl;
    
    for(size_t i=0; i < m_Helper.instances().size();i++) {
        
        InstanceInfo& inst = m_Helper.instances()[i];
        
        if((inst.type == "link" || inst.type == "video") && inst.attributes["thumb"] != "") {
            string f = inst.attributes["thumb"];
            if(isLocalAssset(f)) {
                m_AssetsToCopy.insert(f);
                f = fileName(f);
            }
            string id = "image_" + baseName(f);
            m_HtmlWriter.addAsset("<AssetImage id=\""+id+"\" src=\""+f+"\"/>");
            inst.attributes["thumb"] = id;
        }
       
        if(inst.value == "") continue;
        
        cout << inst.value << endl;

		if(inst.type == "sound" || inst.type == "image" || inst.type == "video" ) {
            
            string f = inst.value;
            if(isLocalAssset(f)) {
                m_AssetsToCopy.insert(f);
                f = fileName(f);
            }
            
            string id = inst.type + "_" + stringReplace(stringReplace(baseName(f), " ", "_"), "%20", "_");
            
			if(inst.type == "sound") {
				m_HtmlWriter.addAsset("<AssetSound id=\""+id+"\" src=\""+f+"\" />");
			}
            
            if(inst.type == "video") {
				m_HtmlWriter.addAsset("<AssetVideo id=\""+id+"\" src=\""+f+"\" />");
			}
            
            if(inst.type == "image") {
				m_HtmlWriter.addAsset("<AssetImage id=\""+id+"\" src=\""+f+"\" />");
			}
        
            inst.value = id;
		}
    }
    
    cout << "-- Collecting Placeholders" << endl;
	
	map<string,string>::iterator placeHolderItr = m_Helper.placeholders().begin();
	while(placeHolderItr != m_Helper.placeholders().end()){
		
        cout << "Found: " << placeHolderItr->second << endl;

		string mtlFile = ModelWriter::getMaterialFile(placeHolderItr->second);
        
        cout << " -- Material File: " << mtlFile << endl;

		if(!mtlFile.empty()){
			set<string> textures = ModelWriter::getTextures(mtlFile);
            cout << " -- Texture Files: " << textures.size() << endl;
			m_AssetsToCopy.insert(textures.begin(),textures.end());
		}

		m_HtmlWriter.addAsset("<AssetObject id=\"object_"+placeHolderItr->first+"\" src=\""+placeHolderItr->first+".obj\" mtl=\""+fileName(mtlFile)+"\" />");
        
		m_AssetsToCopy.insert(mtlFile);
		m_AssetsToCopy.insert(placeHolderItr->second);

		placeHolderItr++;
	}
    
    
    set<string>::iterator fileItr = m_AssetsToCopy.begin();
    while(fileItr != m_AssetsToCopy.end()){
        if(!fileExists(*fileItr)) {
            cout << "Error file " << *fileItr << " could not be located" << endl;
            exit(1);
        }
        
        string dest = m_Config.outputDir+fileName(*fileItr);
        if(fileExists(dest)) {
            cout << "Skipping " << *fileItr << " already exists in output directory" << endl;
			fileItr++;
            continue;
        }
        
        cout << *fileItr << " -> " << dest << endl;
        fileCopy(*fileItr, dest);
          
        fileItr++;
    }
}

void SketchUp2Room::writeGeometry() {
	   
	cout << "Writing world geometry" << endl;
	ModelWriter hull(m_Config.outputDir, m_Config.roomName);
	hull.write(m_Helper.topLevelEntities());

    cout << "Exporting geometry" << endl;
    map<string,SUComponentDefinitionRef>::iterator itr = m_Helper.components().begin();
    while(itr != m_Helper.components().end()) {
        
        string componentName = itr->first;
        
        if(componentName[0] == '!') {
            itr++;
            continue;
		}
        
        m_HtmlWriter.addAsset("<AssetObject id=\"object_"+componentName+"\" src=\""+componentName+".obj\" mtl=\""+componentName+".mtl\" />");
        
		if(fileExists(m_Config.outputDir + componentName + ".obj") && !m_Config.force) {
            cout << "File " << componentName << ".obj exists, skipping..." << endl;
        } else {
			SUEntitiesRef ents = SU_INVALID;
            SUComponentDefinitionGetEntities(itr->second,&ents);
            
            cout << "Writing component " << componentName << endl;
            ModelWriter writer(m_Config.outputDir,componentName);
            writer.write(ents,true);
        }
        
        itr++;
    }
}

void SketchUp2Room::writeShaders() {
	  
    if(!m_Config.shader.empty()) {
		cout << "Baking default shader :) " << endl;
		m_HtmlWriter.setDefaultShader("room_"+ m_Config.roomName+"_"+m_Config.shader);
		
		ShaderWriter shaderWriter(m_Config.outputDir + "room_"+ m_Config.roomName+"_"+m_Config.shader);
		shaderWriter.includeFile(m_Config.shader);
		shaderWriter.writeLights(m_Helper.instances());
    }
}

void SketchUp2Room::writeHtml() {
	   
    if(!m_Config.outputHtml.empty()) {
		m_HtmlWriter.write(m_Helper.instances());
	}
}

int main(int argc, char* argv[])
{
	SketchUp2Room app;
	
    if(app.parseArguments(argc,argv)) {
        return 1;
    }

	app.readFile();

 	app.saveExternals();
	app.writeGeometry();
	app.writeShaders();
	app.writeHtml();
 
 	return 0;
}
