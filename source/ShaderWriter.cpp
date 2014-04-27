#include "stdafx.h"
#include "ShaderWriter.h"

#include <sstream>

ShaderWriter::ShaderWriter(const string& outFile, const string& inFile) {

	m_Shader.open(outFile.c_str());
	
	ifstream templ;
	templ.open(inFile);

	char buff[200];
	while(!templ.eof()) {
		templ.getline(buff,sizeof(buff));
		m_Shader << buff << endl;
	}

	
	m_Shader.precision(3);
	m_Shader.setf( std::ios::fixed, std:: ios::floatfield );
}




void ShaderWriter::writeLights(const vector<InstanceInfo>& instances) {

	m_Shader << "void applySceneLights() {" << endl;

	for(size_t i = 0; i < instances.size(); i ++) {

		InstanceInfo& light = *const_cast<InstanceInfo*>(&instances[i]);


		if(light.type == "light") {
			cout << "--Got light" << endl;

			SUPoint3D pos = {0};
			pos = (pos * light.transform) / g_Scale;

			SUVector3D colour = { 1, 1, 1};

			if(light.value != "") {
				//Parse colour value

				stringstream col(light.value);

				col >> colour.x;
				col >> colour.y;
				col >> colour.z;
			}

			string func = "pointlight";

			string posString;

			stringstream ss;
			ss << "vec3(" << pos.x << "," << pos.y << "," << pos.z << ")";
			posString = ss.str();

			if(light.attributes["func"] != "") {
				func = light.attributes["func"];
			}

			if(light.attributes["cond"] != "") {

				string cond = stringReplace(light.attributes["cond"],"$POS",posString);

				m_Shader << "if(" << cond << ") ";
			}

			m_Shader << func << "(" << posString << ",vec3(" << colour.x << "," << colour.y << "," << colour.z << "),0.00f,20.00f);" << endl;
		}

	}
	
	m_Shader << "}" << endl;
}
