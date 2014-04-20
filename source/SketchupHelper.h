#include "stdafx.h"

class SketchupHelper
{
public:

	//Convert to std::string and released
	static string fromSUString(SUStringRef& str);
	static string materialName(SUMaterialRef);
	static string componentName(SUComponentDefinitionRef);
	static string componentInstanceName(SUComponentInstanceRef);
	static string componentInstanceType(SUComponentInstanceRef);

	static string getFrontFaceMaterialName(SUFaceRef);
	static string getBackFaceMaterialName(SUFaceRef);

	static bool isFrontFaceTextured(SUFaceRef);
	static bool isBackFaceTextured(SUFaceRef);

	static SUModelRef openFile(const string& filename);
	
	
};
