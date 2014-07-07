#include "StdAfx.h"
#include "HtmlWriter.h"

#include "SketchupHelper.h"
#include "ModelWriter.h"


HtmlWriter::HtmlWriter(const Config& config) : m_Config(config)
{
    m_RoomAtributes["draw_glow"] = "false";
    m_RoomAtributes["default_sounds"] = "false";
    m_RoomAtributes["col"] = "0 0 0";

	m_Origin = BaseTransform;
}


HtmlWriter::~HtmlWriter(void)
{
}

void HtmlWriter::addAsset(const string &assetTag) {
    m_Assets.insert(assetTag);
}

void HtmlWriter::setDefaultShader(const string &s) {
    addAsset("<AssetShader id=\"default_shader_id\" src=\"" + s + "\"/>");
    m_DefaultShaderId = "default_shader_id";
}

void HtmlWriter::writeAssets( const vector<InstanceInfo>& instances) {
	m_Output << "<AssetObject id=\"object_hull\" src=\"" << m_Config.roomName << ".obj\" mtl=\"" << m_Config.roomName <<".mtl\" />" << endl;
			
    set<string>::iterator itr = m_Assets.begin();
    while(itr != m_Assets.end()) {
        m_Output << *itr << endl;
        itr++;
    }
}

void HtmlWriter::writeRoomTag(const vector<InstanceInfo>& instances) {
	
	m_Output << "<Room ";
	map<string,string>::iterator itr= m_RoomAtributes.begin();
    while(itr != m_RoomAtributes.end()) {
        m_Output << itr->first << "=\"" << itr->second << "\" ";
        itr++;
    }
    m_Output << m_Origin << ">" << endl;

	m_Output << "<Object id=\"object_hull\" collision_id=\"object_hull\"";
	m_Output << " shader_id=\"" << m_DefaultShaderId << "\"";
	m_Output << " locked=\"true\" "<< TransformIdentity<<" />" << endl;
	
	for(size_t i=0; i < instances.size(); i++) 
		writeInstance(instances[i]);
	
	m_Output << "</Room>" << endl;
}

bool HtmlWriter::write(const vector<InstanceInfo>& instances) {
	  
	m_Title = baseName(m_Config.roomName);

	ifstream templateFile;

	templateFile.open(m_Config.templateName);

	if(!templateFile.good()) 
		templateFile.open(sdkDir() + "/html/" + m_Config.templateName);
	
	if(!templateFile.good()) {
		cerr << "Could not open template file" << endl;
		return false;
	}

	m_Output.open(m_Config.outputDir + m_Config.outputHtml);
	m_Output.precision(3);
	m_Output.setf( std::ios::fixed, std:: ios::floatfield );
    
	for(size_t i=0; i < instances.size(); i++) {

		const InstanceInfo& inst = instances[i];
			
		if(inst.type == "origin") {
			m_Origin = inst.transform;
		}
    }
    
	cout << "Writing room HTML" << endl;


	while(!templateFile.eof()) {

		char buffer[300];
		templateFile.getline(buffer,sizeof(buffer));

		if(strcmp(buffer,"%ROOM_TITLE%") == 0) {
			m_Output << "<title>" << m_Title << "</title>" << endl;
			continue;
		}
		
		if(strcmp(buffer,"%ASSETS%") == 0) {
			writeAssets(instances);
			continue;
		}

		if(strcmp(buffer,"%ROOM_HTML%") == 0) {
			writeRoomTag(instances);
			continue;
		}

		m_Output << buffer << endl;
	}

	m_Output.close();

	cout << "HTML Finished" << endl;

	return true;
}

void HtmlWriter::writeObjectTag(const InstanceInfo& obj) {

	m_Output << "<Object id=\"" << obj.modelId << "\" ";
	m_Output << " shader_id=\"" << obj.getAttribute("shader",m_DefaultShaderId) << "\"";
	m_Output << " locked=\"true\" ";
		
	if(obj.type != "nonsolid" && obj.modelName[0] != '+' ) {
		m_Output << " collision_id=\"" << obj.modelId <<"\" ";
	}

	if(obj.hasAttribute("rotate_speed")){
		m_Output << " rotate_deg_per_sec=\"" << obj.getAttribute("rotate_speed") << "\" rotate_axis=\"" << obj.getAttribute("rotate_axis", "0 0 1") << "\" ";
	}

	writeTransform(m_Output, obj.transform, false);
	m_Output << " scale=\"" << getTransformScale(obj.transform) << "\"/>" << endl;
}

void HtmlWriter::writeLinkTag(const InstanceInfo& obj) {
	
	m_Output << "<Link url=\"" << obj.value << "\" ";
			
	if(obj.hasAttribute("noglow")) {
		m_Output << "draw_glow=\"false\" ";
	}

	if(obj.hasAttribute("autoload")) {
		m_Output << "auto_load=\"true\" ";
	}

	if(obj.hasAttribute("notext")) {
		m_Output << "draw_text=\"false\" ";
	}

	if(obj.hasAttribute("title")) {
		m_Output << "title=\""<<obj.getAttribute("title")<<"\" ";
	}
        
    if(obj.hasAttribute("col")) {
		m_Output << "col=\"" << obj.getAttribute("col", "1 1 1")<<"\" ";
	}
        
    if(obj.hasAttribute("thumb")) {
        m_Output << "thumb_id=\"" << obj.getAttribute("thumb")  << "\" ";
    }
        
	m_Output << obj.transform << "scale=\"" << obj.getAttribute("size","1.8 2.5") << " 1\" />" << endl;
}

void HtmlWriter::writeTextTag(const InstanceInfo& obj) {

	m_Output << (obj.type == "text" ?  "<Text " :"<Paragraph ")  << endl;
	m_Output << "shader_id=\"" << obj.getAttribute("shader",m_DefaultShaderId) << "\" ";

	if(obj.hasAttribute("font_size")){
		m_Output << "font_size=\""<<obj.getAttribute("font_size")<<"\" ";
	}

	if(obj.hasAttribute("col")){
		m_Output << "col=\""<<obj.getAttribute("col")<<"\" ";
	}

	if(obj.hasAttribute("text_col")){
		m_Output << "text_col=\""<<obj.getAttribute("text_col")<<"\" ";
	}

	if(obj.hasAttribute("back_col")){
		m_Output << "back_col=\""<<obj.getAttribute("back_col")<<"\" ";
	}
	if(obj.hasAttribute("back_alpha")){
		m_Output << "back_alpha=\""<<obj.getAttribute("back_alpha")<<"\" ";
	}

	SUPoint3D pos = { 0.0f, -3.5f, 0.0f };
	writeTransform(m_Output,obj.transform,true, pos);

	m_Output << " scale=\"" << getTransformScale(obj.transform) << "\" >";
	m_Output << obj.value;
	m_Output << (obj.type == "text" ?  "</Text>" :"</Paragraph>")  << endl;
}

VOID HtmlWriter::writeImageTag(const InstanceInfo& obj) {
        
    m_Output << "<Image id=\"" << obj.value << "\" ";
	writeTransform(m_Output,obj.transform, false, obj.offset);
	
	m_Output << "shader_id=\"" << obj.getAttribute("shader",m_DefaultShaderId) << "\" ";

	if(obj.hasAttribute("scale")) {
		m_Output << "scale=\"" << obj.getAttribute("scale") << "\" ";
	} else {
	
		m_Output << "scale=\"" << length(xaxis*obj.transform) << " " << length(yaxis*obj.transform) << " 0\" ";
	}

	m_Output <<" />" << endl;    
}

void HtmlWriter::writeSoundTag(const InstanceInfo& obj) {

    m_Output << "<Sound id=\"" << obj.value << "\" rect=\"-100 -100 100 100\"";
	m_Output << " loop=\"" << obj.getAttribute("loop","false") << "\"";
	m_Output << " />" << endl;
}

void HtmlWriter::writeVideoTag(const InstanceInfo& obj) {
	double width = length(xaxis * obj.transform);
	double height = length(yaxis * obj.transform);
	m_Output << "<Video id=\"" << obj.value << "\" " << obj.transform << " ";
	
	if(obj.hasAttribute("thumb")) {
		m_Output << "thumb_id=\"" << obj.getAttribute("thumb") << "\" ";
	}

	m_Output << "scale=\"" << width << " " << height << " " << width << "\" />" << endl;
}


void HtmlWriter::writeInstance(const InstanceInfo& obj) {

	if(obj.modelName[0] != '!') writeObjectTag(obj);
	if(obj.type == "link") writeLinkTag(obj);
	if(obj.type == "text" || obj.type == "paragraph") writeTextTag(obj);
	if(obj.type == "sound") writeSoundTag(obj);
    if(obj.type == "video") writeVideoTag(obj);
    if(obj.type == "image") writeImageTag(obj);
}