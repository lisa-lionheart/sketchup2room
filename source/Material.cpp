#include "StdAfx.h"
#include "Material.h"
#include "SketchupHelper.h"

SUColor black = { 0 }

Material::Material(SUMaterialRef mat): 
	ambient(black),
	diffuse(black),
	specular(black)
{
	SUMaterialGetColor(mat,&diffuse);
	string description = SketchupHelper::materialName(mat);

	vector<string> attributes;

}


Material::~Material(void)
{
}
