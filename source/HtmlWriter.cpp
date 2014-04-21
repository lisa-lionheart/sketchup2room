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



void HtmlWriter::writeAssets( const vector<SUComponentInstanceRef>& instances) {

	map<string,bool> foundComponents;
	for(size_t i=0; i < instances.size(); i++) {

		string type = SketchupHelper::componentInstanceType(instances[i]);;
		
		//Placeholder components
		if(type[0] == '!'){
			continue;
		}
		
		string name = SketchupHelper::componentInstanceName(instances[i]);
		if(name.compare("$origin") == 0) {
			SUComponentInstanceGetTransform(instances[i],(SUTransformation*)&m_Origin);
		}

		if(foundComponents[type] == false) {
			foundComponents[type] = true;
			
			cout << " - Found component: "  << type << endl;
			m_Html << "<AssetObject id=\""<< type <<"\" src=\"" << type << ".obj\" mtl=\"" << type <<".mtl\" />" << endl;
		}
	}
}


bool HtmlWriter::write(SUModelRef model) {
	
	cout << "Writing room HTML" << endl;

	// Get the entity container of the model.
	SUEntitiesRef ents = SU_INVALID;
	size_t numInstances =0;
	
	SUModelGetEntities(model, &ents);
	SUEntitiesGetNumInstances(ents,&numInstances);

	vector<SUComponentInstanceRef> instances(numInstances);
	SUEntitiesGetInstances(ents,numInstances,instances.data(),&numInstances);

	m_Html << "<html>" << endl;
	m_Html << "<head>" << endl;
	m_Html << "<title>SketchUp Room - " << m_Title << "</title>" << endl;
	m_Html << "</head>" << endl;
	m_Html << "<body>" << endl;
	m_Html << "<FireBoxRoom>" << endl;
	m_Html << "<Assets>" << endl;
	m_Html << "<AssetObject id=\"hull\" src=\"" << m_FileName << ".obj\" mtl=\"" << m_FileName <<".mtl\" />" << endl;
	
	if(m_DefaultShader.length() != 0) {
		m_Html << "<AssetShader id=\"default_shader_id\" src=\"" << m_DefaultShader << "\" />" << endl;

	}

	writeAssets(instances);

	m_Html << "</Assets>" << endl;
	m_Html << "<Room ";
	writeTransform(m_Html,BaseTransform*m_Origin,true) << ">" << endl;
	m_Html << "<Object id=\"hull\" collision_id=\"hull\" "<<m_DefaultShaderId<<"locked=\"true\" "<<BaseTransform<<" />" << endl;
	
	for(size_t i=0; i < numInstances; i++) {
		writeObject(instances[i]);
	}
	
	m_Html << "</Room>" << endl;
	m_Html << "</FireBoxRoom>" << endl;
	m_Html << "</body>" << endl;
	m_Html << "</m_Html>" << endl;

	m_Html.close();

	cout << "HTML Finished" << endl;

	return true;
}



bool HtmlWriter::parseInstanceName(const string& name, InstanceMeta& meta) {
	if(name.length() == 0 || name[0] != '$' ) {
		return false;
	}

			
	size_t firstPipe = name.find('|');
	if(firstPipe == -1){
		return true; //No more data
	}
	
	size_t lastPipe = name.find_last_of('|'); 

	meta.type = name.substr(1,firstPipe-1);
	string value = name.substr(lastPipe+1);
	
	for(int i=0; i < value.length(); i++) {
		if(value[i] == '\\' && value[i+1] == 'n') {
			value[i] = ' ';
			value[i+1] = '\n';
		}
	}

	meta.value = value;

	if(lastPipe != firstPipe){
		size_t pos = firstPipe;
		while(pos != -1) {
			size_t nextPos = name.find(',', pos+1);
			string option = name.substr(pos+1, ((nextPos!=-1 && nextPos < lastPipe) ? nextPos : lastPipe) - (pos+1) );
			string value = "true";
						
			size_t equals = option.find('=');
			if(equals != -1) {
				value = option.substr(equals+1);
				option = option.substr(0,equals);
			}

			meta.attributes[option] = value;
			pos = nextPos;
		}
	}
	return true;
}

void HtmlWriter::writeObject(SUComponentInstanceRef obj) {
	Transform t;
	SUComponentInstanceGetTransform(obj, (SUTransformation*)&t);

	string name = SketchupHelper::componentInstanceName(obj);
	string type = SketchupHelper::componentInstanceType(obj);
	
	if(type[0] != '!'){
		m_Html << "<Object id=\"" << type << "\" "<<m_DefaultShaderId<<"locked=\"true\" ";
		
		if(name != "$nonsolid") {
			m_Html << "collision_id=\"" << type <<"\" ";
		}

		m_Html << BaseTransform*t << " scale=\"" << getTransformScale(t) << "\"/>" << endl;
	}

	InstanceMeta meta;
	if(parseInstanceName(name,meta)) {
		
		if(meta.type == "link"){

			m_Html << "<Link url=\"" << meta.value << "\" ";
			
			if(meta.attributes["noglow"] == "true") {
				m_Html << "draw_glow=\"false\" ";
			}

			if(meta.attributes["autoload"] == "true") {
				m_Html << "auto_load=\"true\" ";
			}

			if(meta.attributes["notext"] == "true"){
				m_Html << "draw_text=\"false\" ";
			}

			if(meta.attributes["title"] != ""){
				m_Html << "title=\""<<meta.attributes["title"]<<"\" ";
			}

			m_Html << "scale=\"1.8 2.5 1\" ";
			writeTransform(m_Html,BaseTransform*t,true) << "/>" << endl;
		}

		if(meta.type == "text" || meta.type == "paragraph"){
			
			if(meta.type == "text") {
				m_Html << "<Text ";
			} else {
				m_Html << "<Paragraph ";
			}

			if(meta.attributes["font_size"] != ""){
				m_Html << "font_size=\""<<meta.attributes["font_size"]<<"\" ";
			}

			if(meta.attributes["col"] != ""){
				m_Html << "col=\""<<meta.attributes["col"]<<"\" ";
			}

			if(meta.attributes["text_col"] != ""){
				m_Html << "text_col=\""<<meta.attributes["text_col"]<<"\" ";
			}

			if(meta.attributes["back_col"] != ""){
				m_Html << "back_col=\""<<meta.attributes["back_col"]<<"\" ";
			}
			if(meta.attributes["back_alpha"] != ""){
				m_Html << "back_alpha=\""<<meta.attributes["back_alpha"]<<"\" ";
			}

			writeTransform(m_Html,BaseTransform*t,true) << " scale=\"" << getTransformScale(t) << "\" >";
			m_Html << meta.value;
			if(meta.type == "text") {
				m_Html << "</Text>" << endl;
			} else {
				m_Html << "</Paragraph>" << endl;
			}
 		}
	}
}