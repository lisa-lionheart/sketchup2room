
#include "stdafx.h"
#include "SketchupHelper.h"
#include <fstream>


#include "Geometry.hpp"
#include "ModelWriter.h"

const float g_Scale = 40.0f;
const Transform TransformIdentity = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
};

const SUVector3D xaxis = { 1.0, 0.0, 0.0 };
const SUVector3D yaxis = { 0.0, 1.0, 0.0 };
const SUVector3D zaxis = { 0.0, 0.0, 1.0 };

//Transform sketchup space to JansusVR space
const Transform BaseTransform = {
	-1, 0, 0, 0,
	0, 0, 1, 0,
	0, 1, 0, 0,
	0, 0, 0, 1
};


//For placeholder meshes, basically undoes basetransform
const Transform PlaceholderTransform = {
	-1, 0, 0, 0,
	0, 0, 1, 0,
	0, -1, 0, 0,
	0, 0, 0, 1
};

string SketchupHelper::fromSUString(SUStringRef& str) {
	size_t len;
	SUStringGetUTF8Length(str,&len);

	char buff[300] = {0};
	SUStringGetUTF8(str,300,buff,&len);
	SUStringRelease(&str);
	return buff;
}

string SketchupHelper::materialName(SUMaterialRef mat) {
	
	if(mat.ptr == 0) {
		return "";
	}
	
	SUStringRef str = SU_INVALID;
	SUStringCreate(&str);
	SUResult res = SUMaterialGetName(mat,&str);
	if(res == SU_ERROR_NONE) {
		string str2 = fromSUString(str);
		
		return str2;
	}


    return "";
}

string SketchupHelper::getFrontFaceMaterialName(SUFaceRef face) {

	SUMaterialRef mat = SU_INVALID;
	SUFaceGetFrontMaterial(face,&mat);
	if(mat.ptr == 0){
		return "";
	}
	return materialName(mat);
}


string SketchupHelper::getBackFaceMaterialName(SUFaceRef face) {

	SUMaterialRef mat = SU_INVALID;
	SUFaceGetBackMaterial(face,&mat);

	if(mat.ptr == 0){
		return "";
	}
	return materialName(mat);
}


string SketchupHelper::componentName(SUComponentDefinitionRef ref) {

	SUStringRef str = SU_INVALID;
	SUStringCreate(&str);
	SUComponentDefinitionGetName(ref,&str);
	return fromSUString(str);
}

string SketchupHelper::componentInstanceName(SUComponentInstanceRef ref) {

	SUStringRef str = SU_INVALID;
	SUStringCreate(&str);
	SUComponentInstanceGetName(ref,&str);
	return fromSUString(str);
}

string SketchupHelper::componentInstanceType(SUComponentInstanceRef ref) {

	SUComponentDefinitionRef def = SU_INVALID;
	SUComponentInstanceGetDefinition(ref,&def);
	return componentName(def);
}

bool SketchupHelper::isFrontFaceTextured(SUFaceRef face) {
	SUTextureRef tex = SU_INVALID;
	SUMaterialRef mat = SU_INVALID;

	SUFaceGetFrontMaterial(face,&mat);
	SUMaterialGetTexture(mat,&tex);

	return tex.ptr != 0;
}

bool SketchupHelper::isBackFaceTextured(SUFaceRef face) {
	SUTextureRef tex = SU_INVALID;
	SUMaterialRef mat = SU_INVALID;

	SUFaceGetBackMaterial(face,&mat);
	SUMaterialGetTexture(mat,&tex);

	return tex.ptr != 0;
}

SketchupHelper::SketchupHelper(){
    
    m_Model.ptr = 0;
    m_TopLevelEnts.ptr = 0;
}

bool SketchupHelper::openFile(const string& filename) {
	SUResult res = SUModelCreateFromFile(&m_Model, filename.c_str());
    if(res != SU_ERROR_NONE) {
        return false;
    }
    
    
    cout << "- Processing file" << filename;
    
    SUModelGetEntities(m_Model, &m_TopLevelEnts);
    getInstancesRecursive(m_TopLevelEnts);
	return true;
}


bool SketchupHelper::parseInstanceName(const string& name, InstanceInfo& meta) {

	if(name.length() == 0 || name[0] != '$' ) {
		meta.type = "solid"; //Default type
		return false;
	}
		
	size_t firstPipe = name.find('|');
	if(firstPipe == -1){
		meta.type = name.substr(1);
		return true; //No more data
	}
	
	size_t lastPipe = name.find_last_of('|'); 

	meta.type = name.substr(1,firstPipe-1);
	string value = name.substr(lastPipe+1);
	
	for(size_t i=0; i < value.length(); i++) {
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

void SketchupHelper::getInstancesRecursive(SUEntitiesRef ents, Transform parentTransform) {
	
	size_t count =0;
	SUEntitiesGetNumInstances(ents,&count);
	vector<SUComponentInstanceRef> instances(count);
	SUEntitiesGetInstances(ents,count,instances.data(),&count);

	m_Instances.reserve(m_Instances.size()+count);

	for(size_t i =0; i < instances.size(); i++) {

		InstanceInfo info;

		Transform t;
		SUComponentInstanceGetTransform(instances[i],(SUTransformation*)&t);

		info.instance = instances[i];
		info.modelName = componentInstanceType(instances[i]);
		info.modelId = "object_" + info.modelName;

		

		parseInstanceName(componentInstanceName(instances[i]),info);


		if(info.modelName[0] == '$') {

			string filename = info.modelName.substr(1);
			info.modelName = baseName(filename);
			info.modelId = "object_"+info.modelName;

			t = (t * PlaceholderTransform);

			m_Placeholders[info.modelName] = filename;
		} else {

			SUComponentDefinitionRef def = SU_INVALID;
			SUComponentInstanceGetDefinition(info.instance, &def);
        
			m_Components[info.modelName] = def;
		}
		
		info.transform = parentTransform*t;

		m_Instances.push_back(info);
	}


	SUEntitiesGetNumGroups(ents,&count);
	vector<SUGroupRef> groups(count);
	SUEntitiesGetGroups(ents,count,groups.data(),&count);

	for(size_t i =0; i < groups.size(); i++) {

		SUGroupGetEntities(groups[i],&ents);

		Transform t;
		SUGroupGetTransform(groups[i],(SUTransformation*)&t);
		getInstancesRecursive(ents, parentTransform*t);
	}

}

