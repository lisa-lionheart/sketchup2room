#include "StdAfx.h"
#include "HtmlWriter.h"

#include "SketchupHelper.h"
#include "ModelWriter.h"


HtmlWriter::HtmlWriter(const string& outDir, const string& fileName)
{
	m_Title = "Untitled";
	m_FileName = fileName;
	m_OutputDir = outDir;
	m_Origin = TransformIdentity;

	m_Html.open(outDir + fileName + ".html");
	m_Html.precision(3);
	m_Html.setf( std::ios::fixed, std:: ios::floatfield );
}


HtmlWriter::~HtmlWriter(void)
{
}



void HtmlWriter::writeAssets( const vector<InstanceInfo>& instances) {

	map<string,bool> foundModels;


	for(size_t i=0; i < instances.size(); i++) {

		const InstanceInfo& inst = instances[i];

		
		//Placeholder components
		if(inst.modelName[0] == '!'){
			continue;
		}
		
		if(inst.type == "origin") {
			m_Origin = inst.transform;
		}

		if(foundModels[inst.modelName] == false) {
			foundModels[inst.modelName] = true;
			
			cout << " - Found component: "  << inst.modelName << endl;
			m_Html << "<AssetObject id=\""<< inst.modelId <<"\" src=\"" << inst.modelName << ".obj\" mtl=\"" << inst.modelName <<".mtl\" />" << endl;
		}
	}
}


bool HtmlWriter::write(const vector<InstanceInfo>& instances) {
	
	cout << "Writing room HTML" << endl;


	m_Html << "<html>" << endl;
	m_Html << "<head>" << endl;
	m_Html << "<title>SketchUp Room - " << m_Title << "</title>" << endl;
	m_Html << "</head>" << endl;
	m_Html << "<body>" << endl;
	m_Html << "<p>Put your goggles on ;)</p>" << endl;
	m_Html << "<!--" << endl;
	m_Html << "<FireBoxRoom>" << endl;
	m_Html << "<Assets>" << endl;
	m_Html << "<AssetObject id=\"hull\" src=\"" << m_FileName << ".obj\" mtl=\"" << m_FileName <<".mtl\" />" << endl;
	
	if(m_DefaultShader.length() != 0) {
		m_Html << "<AssetShader id=\"default_shader_id\" src=\"" << m_DefaultShader << "\" />" << endl;

	}

	writeAssets(instances);

	m_Html << "</Assets>" << endl;
	m_Html << "<Room draw_glow=\"false\" default_sounds=\"false\" col=\"0 0 0\" ";
	writeTransform(m_Html,m_Origin,true) << ">" << endl;
	m_Html << "<Object id=\"hull\" collision_id=\"hull\" "<<m_DefaultShaderId<<"locked=\"true\" "<<BaseTransform<<" />" << endl;
	
	for(size_t i=0; i < instances.size(); i++) {
		writeObject(instances[i]);
	}
	
	m_Html << "</Room>" << endl;
	m_Html << "</FireBoxRoom>" << endl;
	m_Html << "-->" << endl;
	m_Html << "</body>" << endl;
	m_Html << "</m_Html>" << endl;

	m_Html.close();

	cout << "HTML Finished" << endl;

	return true;
}



void HtmlWriter::writeObject(const InstanceInfo& _obj) {

	InstanceInfo& obj = *const_cast<InstanceInfo*>(&_obj);

	if(obj.modelName[0] != '!'){
		m_Html << "<Object id=\"" << obj.modelId << "\" "<< m_DefaultShaderId <<"locked=\"true\" ";
		
		if(obj.type != "nonsolid") {
			m_Html << "collision_id=\"" << obj.modelId <<"\" ";
		}

		m_Html << obj.transform << " scale=\"" << getTransformScale(obj.transform) << "\"/>" << endl;
	}

	if(obj.type == "link"){

		m_Html << "<Link url=\"" << obj.value << "\" ";
			
		if(obj.attributes["noglow"] == "true") {
			m_Html << "draw_glow=\"false\" ";
		}

		if(obj.attributes["autoload"] == "true") {
			m_Html << "auto_load=\"true\" ";
		}

		if(obj.attributes["notext"] == "true"){
			m_Html << "draw_text=\"false\" ";
		}

		if(obj.attributes["title"] != ""){
			m_Html << "title=\""<<obj.attributes["title"]<<"\" ";
		}

		m_Html << "scale=\"1.8 2.5 1\" ";
		writeTransform(m_Html,obj.transform,true) << "/>" << endl;
	}

	if(obj.type == "text" || obj.type == "paragraph"){
			
		if(obj.type == "text") {
			m_Html << "<Text ";
		} else {
			m_Html << "<Paragraph ";
		}

		if(obj.attributes["font_size"] != ""){
			m_Html << "font_size=\""<<obj.attributes["font_size"]<<"\" ";
		}

		if(obj.attributes["col"] != ""){
			m_Html << "col=\""<<obj.attributes["col"]<<"\" ";
		}

		if(obj.attributes["text_col"] != ""){
			m_Html << "text_col=\""<<obj.attributes["text_col"]<<"\" ";
		}

		if(obj.attributes["back_col"] != ""){
			m_Html << "back_col=\""<<obj.attributes["back_col"]<<"\" ";
		}
		if(obj.attributes["back_alpha"] != ""){
			m_Html << "back_alpha=\""<<obj.attributes["back_alpha"]<<"\" ";
		}

		writeTransform(m_Html,obj.transform,true) << " scale=\"" << getTransformScale(obj.transform) << "\" >";
		m_Html << obj.value;
		if(obj.type == "text") {
			m_Html << "</Text>" << endl;
		} else {
			m_Html << "</Paragraph>" << endl;
		}
 	}
	
}