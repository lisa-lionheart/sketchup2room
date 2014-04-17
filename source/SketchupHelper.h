
#include "stdafx.h"
#include <string>

using std::string;

class SketchupHelper
{
	mutable SUModelRef m_Model;

	float m_Scale;
	
	string m_outputDir;

	string fromSUString(SUStringRef str);
	string materialName(SUMaterialRef);

public:
	SketchupHelper();

	void setOutputDir(const string& dir);
	bool openFile(const string& filename);


	void exportMaterials(const string& filename);

	/**
		Export all of the faces as a single HULL file
	*/
	bool exportHull(const string& filename);

	/**
	*/
	void writeHtmlFile(const string& htmlFile, const string& fileName);

};
