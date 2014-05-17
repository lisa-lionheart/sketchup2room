#include "StdAfx.h"
#include "Material.h"
#include "SketchupHelper.h"
#include <sstream>

void stringSplit(string in, char delimiter, /*out*/ vector<string>& out) {

	while(true) {

		size_t i = in.find_first_of(delimiter);
		if(i==-1) {
			out.push_back(in);
			return;
		}

		out.push_back(in.substr(0,i));
		in = in.substr(i+1);
	}
}

SUColor black = { 0, 0, 0, 1 };

Material::Material(SUMaterialRef mat) : 
	ambient(black),
	diffuse(black),
	specular(black)
{
	rawMaterial = mat;

	SUMaterialGetColor(mat,&diffuse);
	fullName = SketchupHelper::materialName(mat);

	SUMaterialGetOpacity(mat,&alpha);

	vector<string> attributes;
	stringSplit(fullName, '|', attributes);

	name = attributes.front();

	for(size_t i=0; i < name.length(); i++){

		if(name[i] == ' '){
			name[i] = '_';
		}
	}

	for(size_t i =0; i < attributes.size(); i++){

		if(attributes[i].substr(0,8) == "ambient=") {			
			stringstream col(attributes[i].substr(8));
			float val;
			col >> val;
			ambient.red = val * 255;
			col >> val;
			ambient.green = val * 255;
			col >> val;
			ambient.blue = val * 255;
		}

		if(attributes[i].substr(0,9) == "specular=") {			
			stringstream col(attributes[i].substr(9));
			float val;
			col >> val;
			specular.red = val * 255;
			col >> val;
			specular.green = val * 255;
			col >> val;
			specular.blue = val * 255;
		}
	}

}


Material::~Material(void)
{
}

void Material::saveTextures(const string& dir) {
	SUTextureRef tex = SU_INVALID;
	if(SUMaterialGetTexture(rawMaterial ,&tex) == SU_ERROR_NONE) {
		SUStringRef fileName = SU_INVALID;
		SUStringCreate(&fileName);
		if(SUTextureGetFileName(tex,&fileName) == SU_ERROR_NONE) {
			string f = SketchupHelper::fromSUString(fileName);

			for(int i=0; i < f.length(); i ++){
				if(f[i] == ' ') f[i] = '_';
			}

			diffuseTextureName = f;

			SUTextureWriteToFile(tex,(dir+f).c_str());
		}
	}
}


void Material::write(ostream& out) const {

	out << "newmtl " <<  name << endl;		
	//out << "illium 4" << endl;

	out << "Ka " << (ambient.red / 255.0f) << " " << (ambient.green/255.0f) << " " << (ambient.blue/255.0f) << endl; 
	out << "Kd " << (diffuse.red / 255.0f) << " " << (diffuse.green/255.0f) << " " << (diffuse.blue/255.0f) << endl; 
	out << "Ks " << (specular.red / 255.0f) << " " << (specular.green/255.0f) << " " << (specular.blue/255.0f) << endl; 
		
	if(alpha != 1.0) {
		out << "d " << alpha << endl;
		out << "Tr " << alpha << endl;
	}

	if(!diffuseTextureName.empty()) {
		out << "map_Kd " << diffuseTextureName << endl;
	}	
	/*
	SUTextureRef tex = SU_INVALID;
	if(SUMaterialGetTexture(mat,&tex) == SU_ERROR_NONE) {
		SUStringRef fileName = SU_INVALID;
		SUStringCreate(&fileName);
		if(SUTextureGetFileName(tex,&fileName) == SU_ERROR_NONE) {
			string f = SketchupHelper::fromSUString(fileName);

			for(int i=0; i < f.length(); i ++){
				if(f[i] == ' ') f[i] = '_';
			}

			SUTextureWriteToFile(tex,(m_OutputDir+f).c_str());

			out << "Tf 1.00 1.00 1.00" << endl;
			//mtlFile << "map_Ka " << f << endl;
			out << "map_Kd " << f << endl;

			out << "Ni 1.00" << endl;
			out << "Ks 1.00 1.00 1.00" << endl;
			out << "Ns 0.00" << endl;
		}
	} */
}