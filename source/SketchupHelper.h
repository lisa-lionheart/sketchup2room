
#include "stdafx.h"
#include <string>

using std::string;



class SketchupHelper
{
	mutable SUModelRef m_Model;

	float m_Scale;
	string m_outputDir;
	vector<string> m_Components;

	SUTransformation m_Origin;

	//Convert to std::string and release
	string fromSUString(SUStringRef& str);

	string materialName(SUMaterialRef);
	string materialName(SUFaceRef);
	string componentName(SUComponentDefinitionRef);
	string componentInstanceName(SUComponentInstanceRef);
	string componentInstanceType(SUComponentInstanceRef);


	bool exportEntitiesGeometry(SUEntitiesRef, const string& filename);

public:
	SketchupHelper();

	void setOutputDir(const string& dir);
	bool openFile(const string& filename);


	void exportMaterials(const string& filename);

	void proccessInstances();
	/**
		Export all of the faces as a single HULL file
	*/
	bool exportHull(const string& filename);

	/**
	*/
	void writeHtmlFile(const string& htmlFile, const string& fileName);

};
