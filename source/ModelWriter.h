#pragma once

#include "stdafx.h"
#include "Geometry.hpp"
#include <fstream>
#include <set>

class ModelWriter;

typedef void (ModelWriter::*VisitorFunc)(SUFaceRef, const Transform&);

class ModelWriter
{
	
	ofstream m_ObjFile;
	ofstream m_MtlFile;

	size_t m_VerticesUsed;

	map<void*,SUMeshHelperRef> m_MeshHelpers;
	set<void*> m_Materials;

	string m_OutputDir;
	
	//Visting fucntions
	void makeHelper(SUFaceRef, const Transform&);
	void gatherMaterials(SUFaceRef, const Transform&);
	void writeVertices(SUFaceRef, const Transform&);
	void writeFaces(SUFaceRef, const Transform&);

	bool shouldWriteFrontFace(SUFaceRef);
	bool shouldWriteBackFace(SUFaceRef);

	void visitFaces(SUEntitiesRef, VisitorFunc, bool recursive=false, Transform = TransformIdentity);

public:
	ModelWriter(const string& directory, const string& name);
	~ModelWriter(void);


	void write(SUEntitiesRef, bool recursive = false);
	void write(SUModelRef);

	static void exportComponents(SUModelRef, const string& dir, bool overwrite = false);

};

