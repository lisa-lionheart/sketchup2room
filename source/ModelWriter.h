#pragma once

#include "stdafx.h"
#include "Geometry.hpp"
#include <fstream>
#include <set>
#include <sstream>

class ModelWriter;

typedef void (ModelWriter::*VisitorFunc)(SUFaceRef, const Transform&);


class ModelWriter
{
	
	ofstream m_ObjFile;
	ofstream m_MtlFile;

	string m_CurrentMat;
	map<string,SUMaterialRef> m_Materials;

	stringstream m_Faces;

	vector<SUPoint3D> m_UniquePositions;
	vector<SUPoint3D> m_UniqueTextures;
	vector<SUVector3D> m_UniqueNormals;

	string m_OutputDir;
	
	template<typename T>
	inline size_t getOrInsert(vector<T>& vec, T& val) {
		for(size_t i = 0; i<vec.size(); i++) {
			if(vec[i] == val){
				return i+1;
			}
		}
		vec.push_back(val);
		return vec.size();
	}

	size_t getIndexedPos( SUPoint3D& pos) { return getOrInsert(m_UniquePositions,pos); }
	size_t getIndexedTex( SUPoint3D& pos) { return getOrInsert(m_UniqueTextures,pos); }
	size_t getIndexedNormal( SUVector3D& pos) { return getOrInsert(m_UniqueNormals, pos / length(pos)); }
	

	//Visting fucntions
	void makeHelper(SUFaceRef, const Transform&);
	void gatherMaterials(SUFaceRef, const Transform&);
	void writeVertices(SUFaceRef, const Transform&);
	void writeFaces(SUFaceRef, const Transform&);
	void writeFace(SUMeshHelperRef mesh, const Transform& transform, bool front, bool textured);

	bool shouldWriteFrontFace(SUFaceRef);
	bool shouldWriteBackFace(SUFaceRef);

	void visitFaces(SUEntitiesRef, VisitorFunc, bool recursive=false, Transform = TransformIdentity);

public:
	ModelWriter(const string& directory, const string& name);
	~ModelWriter(void);

	static string getMaterialFile(const string& objFile);
	static set<string> getTextures(const string& mtlFile);


	void write(SUEntitiesRef, bool recursive = false);
};

