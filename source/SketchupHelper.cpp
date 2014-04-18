
#include "stdafx.h"
#include "SketchupHelper.h"
#include <fstream>

#include <Windows.h>

const float g_Scale = 40.0f;

string currentDir() {
	char buffer[500];
	GetCurrentDirectoryA(500,buffer);
	return buffer;
}

void makeDir(string dir) {
	CreateDirectoryA(dir.c_str(),NULL);
}

SketchupHelper::SketchupHelper() {
	SUInitialize();

	memset(&m_Origin,0,sizeof(m_Origin));

	m_Origin.values[0] = 1;
	m_Origin.values[5] = 1;
	m_Origin.values[10] = 1;
	m_Origin.values[15] = 1;

	m_outputDir = currentDir()+ "/";
}

void SketchupHelper::setOutputDir(const string& dir) {
	m_outputDir = currentDir() + "/" + dir + "/";
	makeDir(currentDir() + "/" + dir);
	cout << "Output directory is " << m_outputDir << endl;
}

void SketchupHelper::exportMaterials(const string& filename) {
	size_t count =0;
	SUModelGetNumMaterials(m_Model,&count);
	vector<SUMaterialRef> materials(count);
	SUModelGetMaterials(m_Model,count,&materials[0],&count);

	ofstream mtlFile(m_outputDir+filename);

	for(size_t i=0; i < count; i ++) {
		cout << "Writing: " << materialName(materials[i]) << endl;
		mtlFile << "newmtl " << materialName(materials[i]) << endl;
		
		mtlFile << "illium 4" << endl;

		SUColor ambient;
		SUMaterialGetColor(materials[i],&ambient);
		mtlFile << "Ka " << (ambient.red / 255.0f) << " " << (ambient.green/255.0f) << " " << (ambient.blue/255.0f) << endl; 
		mtlFile << "Kd " << (ambient.red / 255.0f) << " " << (ambient.green/255.0f) << " " << (ambient.blue/255.0f) << endl; 
				
		SUTextureRef tex = SU_INVALID;
		if(SUMaterialGetTexture(materials[i],&tex) == SU_ERROR_NONE) {
			SUStringRef fileName = SU_INVALID;
			SUStringCreate(&fileName);
			if(SUTextureGetFileName(tex,&fileName) == SU_ERROR_NONE) {
				string f = fromSUString(fileName);
				SUTextureWriteToFile(tex,(m_outputDir+f).c_str());

				mtlFile << "Tf 1.00 1.00 1.00" << endl;
				//mtlFile << "map_Ka " << f << endl;
				mtlFile << "map_Kd " << f << endl;

				mtlFile << "Ni 1.00" << endl;
				mtlFile << "Ks 1.00 1.00 1.00" << endl;
				mtlFile << "Ns 0.00" << endl;
			}
		} 
	}
}

string SketchupHelper::fromSUString(SUStringRef& str) {
	size_t len;
	SUStringGetUTF8Length(str,&len);

	char buff[300] = {0};
	SUStringGetUTF8(str,300,buff,&len);

	SUStringRelease(&str);
	return buff;
}

string SketchupHelper::materialName(SUMaterialRef mat) {
	SUStringRef str = SU_INVALID;
	SUStringCreate(&str);
	SUResult res =SUMaterialGetName(mat,&str);
	if(res == SU_ERROR_NONE) {
		return fromSUString(str);
	}

	char buf[20];
	return itoa((int)mat.ptr,buf,16);
}

string SketchupHelper::materialName(SUFaceRef face) {

	SUMaterialRef mat = SU_INVALID;
	SUFaceGetFrontMaterial(face,&mat);
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

bool SketchupHelper::openFile(const string& filename) {
	SUModelRef newModel = SU_INVALID;
	SUResult res = SUModelCreateFromFile(&newModel, filename.c_str());
	if (res != SU_ERROR_NONE){

		return false;
	}

	m_Model=newModel;

	
	SUModelUnits units;
	SUModelGetUnits(m_Model,&units);
	/*
	switch(units){
	case SUModelUnits_Inches:
		cout << "Units are inches" << endl;
		m_Scale = 39.3700787;
		break;
	case SUModelUnits_Feet:
		
		cout << "Units are feet" << endl;
		m_Scale = 3.2808399;
		break;
	case SUModelUnits_Millimeters:
		
		cout << "Units are millimeters" << endl;
		m_Scale= 10.0f;
		break;
	case SUModelUnits_Centimeters:
		
		cout << "Units are centimeters" << endl;
		m_Scale= 1.0f;
		break;
	case SUModelUnits_Meters:
		
		cout << "Units are meters" << endl;
		m_Scale= 10.0f * 4;
		break;
	}*/

	m_Scale= 10.0f * 4;
	
	return true;
}


bool SketchupHelper::exportHull(const string& filename) {
	// Get the entity container of the model.
	SUEntitiesRef entities = SU_INVALID;
	SUModelGetEntities(m_Model, &entities);

	return exportEntitiesGeometry(entities,filename);
}

bool SketchupHelper::exportEntitiesGeometry(SUEntitiesRef entities, const string& filename) {

	// Get all the faces from the entities object
	size_t faceCount = 0;
	SUEntitiesGetNumFaces(entities, &faceCount);
	
	if (faceCount == 0) {
		cerr << "Model has no faces :(";
		return false;
	}
		
	cout << "Model has " << faceCount << " faces" << endl;

	ofstream objFile;
	objFile.open(m_outputDir+filename);
	
	objFile << "# File generated by sketchup2room" << endl;

	objFile << "mtllib materials.mtl" << endl;

	int cumlativeVerticies = 1;
	vector<int> vertexStart(faceCount);
	vector<SUFaceRef> faces(faceCount);
	vector<SUMeshHelperRef> faceMeshes(faceCount);
	
	SUEntitiesGetFaces(entities, faceCount, &faces[0], &faceCount);
	

	//Generate meshes
	for(size_t i=0; i < faceCount; i++) {
		SUMeshHelperCreate(&faceMeshes[i],faces[i]);
	}

	//Ouput Vertices
	for(size_t i=0; i < faceCount; i++) {

		if(materialName(faces[i]) == "$nodraw") {
			continue;
		}

		objFile << "# Verts for Face " << i << endl;

		size_t vertexCount = 0;
		SUMeshHelperGetNumVertices(faceMeshes[i],&vertexCount);
		
		vector<SUPoint3D> verts(vertexCount);
		SUMeshHelperGetVertices(faceMeshes[i],vertexCount,&verts[0],&vertexCount);
		
		vector<SUVector3D> normals(vertexCount);
		SUMeshHelperGetNormals(faceMeshes[i],vertexCount,&normals[0],&vertexCount);
		
		vector<SUPoint3D> tcs(vertexCount);
		SUMeshHelperGetFrontSTQCoords(faceMeshes[i],vertexCount,&tcs[0],&vertexCount);


		for(size_t j=0; j < vertexCount; j++) {
			objFile << "v " << -verts[j].x / m_Scale << " " << verts[j].z / m_Scale << " " << verts[j].y / m_Scale << endl;
			objFile << "vt " << tcs[j].x << " " << tcs[j].y << endl;
			objFile << "vn " << -normals[j].x << " " << normals[j].z << " " << normals[j].y << endl;
		}
		
		vertexStart[i] = cumlativeVerticies;
		cumlativeVerticies += vertexCount;
	}

	for(size_t i=0; i < faceCount; i++) {

		if(materialName(faces[i]) == "$nodraw") {
			continue;
		}
		
		size_t triCount = 0;
		SUMeshHelperGetNumTriangles(faceMeshes[i],&triCount);

		size_t gotIndices = 0;
		vector<size_t> indicies(triCount*3);
		SUResult res = SUMeshHelperGetVertexIndices(faceMeshes[i],triCount*3,indicies.data(),&gotIndices);
		assert(res == SU_ERROR_NONE);
		
		size_t s = vertexStart[i];

		objFile << "# Face " << i << endl;

		SUMaterialRef mat = SU_INVALID;
		SUFaceGetFrontMaterial(faces[i],&mat);

		objFile << "usemtl " << materialName(mat) << endl;

		for(size_t j=0; j < triCount; j++) {
		
			objFile << "f ";
			objFile << (s + indicies[(j*3)+0]) << "/" << (s + indicies[(j*3)+0]) << "/" << (s + indicies[(j*3)+0])<< " ";
			objFile << (s + indicies[(j*3)+1]) << "/" << (s + indicies[(j*3)+1]) << "/" << (s + indicies[(j*3)+1]) << " ";
			objFile << (s + indicies[(j*3)+2]) << "/" << (s + indicies[(j*3)+2]) << "/" << (s + indicies[(j*3)+2]) << endl;
		}

		objFile << endl;
	}

	return true;
}

ostream& operator<<(ostream& html, SUTransformation t) {

	html << "pos=\"" << -t.values[12] / g_Scale << " " << t.values[14] / g_Scale << " " << t.values[13]/ g_Scale << "\" "; 
		
	html << "xdir=\"" << -t.values[0] << " " << t.values[2] << " " << t.values[1] << "\" "; 
	html << "ydir=\"" << -t.values[8] << " " << t.values[10] << " " << t.values[9] << "\" "; 
	html << "zdir=\"" << -t.values[4] << " " << t.values[6] << " " << t.values[5] << "\" "; 

	return html;
}

void SketchupHelper::writeHtmlFile(const string& htmlFile, const string& fileName) {

	
	// Get the entity container of the model.
	SUEntitiesRef entities = SU_INVALID;
	SUModelGetEntities(m_Model, &entities);

	size_t numInstances =0;
	SUEntitiesGetNumInstances(entities,&numInstances);

	vector<SUComponentInstanceRef> instances(numInstances);
	SUEntitiesGetInstances(entities,numInstances,instances.data(),&numInstances);

	ofstream html(m_outputDir+htmlFile);


	html << "<html>" << endl;
	html << "<head>" << endl;
	html << "<title>SketchUp Room - " << fileName << "</title>" << endl;
	html << "</head>" << endl;
	html << "<body>" << endl;
	html << "<FireBoxRoom>" << endl;
	html << "<Assets>" << endl;
	html << "<AssetObject id=\"hull\" src=\"" << fileName << ".obj\" mtl=\"" << fileName <<".mtl\" />" << endl;
	
	for(size_t i=0; i < m_Components.size(); i++){
		html << "<AssetObject id=\""<< m_Components[i] <<"\" src=\"" << m_Components[i] << ".obj\" mtl=\"" << fileName <<".mtl\" />" << endl;
	}	
	
	html << "</Assets>" << endl;
	html << "<Room " << m_Origin << ">" << endl;
	html << "<Object id=\"hull\" collision_id=\"hull\" locked=\"true\" pos=\"0 0 0\" />" << endl;
	
	
	for(size_t i=0; i < numInstances; i++) {
		SUTransformation t;
		SUComponentInstanceGetTransform(instances[i],&t);
		string name = componentInstanceName(instances[i]);
		string type = componentInstanceType(instances[i]);
		
		html << "<Object id=\"" << type << "\" locked=\"true\" " << t << "/>" << endl;

		if(name[0] == '$' ) {
			
			size_t firstColon = name.find(':');
			if(firstColon == -1){
				continue;
			}

			size_t lastColon = name.find_last_of(':'); 

			string op = name.substr(1,firstColon-1);
			if(op == "link"){

				string url = name.substr(lastColon+1);

				html << "<Link url=\"" << url << "\" ";

				if(lastColon != firstColon){
					size_t pos = firstColon+1;
					while(pos != -1) {
						size_t nextPos = name.find(',', pos+1);
						
						string option = name.substr(pos, ((nextPos!=-1) ? nextPos : lastColon) - pos );
						string value;
						
						size_t equals = option.find('=');
						if(equals != -1) {
							value = option.substr(equals+1);
							option = option.substr(0,equals-1);
						}

						if(option == "noglow") {
							html << "draw_glow=\"false\" ";
						}

						if(option == "autoload") {
							html << "auto_load=\"true\" ";
						}

						if(option == "title"){
							html << "title=\""<<value<<"\" ";
						}

						pos = nextPos;
					}
				}

				html << "scale=\"1.8 2.5 1\" " << t << "/>" << endl;

			}

		}
	}
	
	
	html << "</Room>" << endl;
	html << "</FireBoxRoom>" << endl;
	html << "</body>" << endl;
	html << "</html>" << endl;

	html.close();
}


void SketchupHelper::proccessInstances(){


	// Get the entity container of the model.
	SUEntitiesRef entities = SU_INVALID;
	SUModelGetEntities(m_Model, &entities);

	size_t numInstances =0;
	SUEntitiesGetNumInstances(entities,&numInstances);

	vector<SUComponentInstanceRef> instances(numInstances);
	SUEntitiesGetInstances(entities,numInstances,instances.data(),&numInstances);


	map<string,bool> foundComponents;

	for(size_t i=0; i < numInstances; i++) {

		string name = componentInstanceName(instances[i]);
		string type = componentInstanceType(instances[i]);;
		
		cout << "Found component: " << name << " type: " << type << endl;

		if(name.compare("$origin") == 0) {
			cout << "Set origin" << endl;
			SUComponentInstanceGetTransform(instances[i],&m_Origin);
		}

		if(foundComponents[type] == false) {
			
			SUComponentDefinitionRef component = SU_INVALID;
			SUComponentInstanceGetDefinition(instances[i],&component);

			SUEntitiesRef entities = SU_INVALID;
			SUComponentDefinitionGetEntities(component,&entities);

			exportEntitiesGeometry(entities, type + ".obj");
			
			foundComponents[type] = true;
			m_Components.push_back(type);
		}
	}
}