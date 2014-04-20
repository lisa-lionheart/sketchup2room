
#include "stdafx.h"
#include "SketchupHelper.h"
#include <fstream>

#include <Windows.h>
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
	SUResult res =SUMaterialGetName(mat,&str);
	if(res == SU_ERROR_NONE) {
		string str2 = fromSUString(str);
		for(size_t i=0; i < str2.length(); i++){

			if(str2[i] == ' '){
				str2[i] = '_';
			}
		}
		return str2;
	}


	char buf[20];
	_itoa_s((int)mat.ptr,buf,16);
	return buf;
}

string SketchupHelper::getFrontFaceMaterialName(SUFaceRef face) {

	SUMaterialRef mat = SU_INVALID;
	SUFaceGetFrontMaterial(face,&mat);
	if(mat.ptr == ""){
		return "";
	}
	return materialName(mat);
}


string SketchupHelper::getBackFaceMaterialName(SUFaceRef face) {

	SUMaterialRef mat = SU_INVALID;
	SUFaceGetBackMaterial(face,&mat);

	if(mat.ptr == ""){
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

SUModelRef SketchupHelper::openFile(const string& filename) {
	SUModelRef newModel = SU_INVALID;
	SUResult res = SUModelCreateFromFile(&newModel, filename.c_str());

	return newModel;
}



