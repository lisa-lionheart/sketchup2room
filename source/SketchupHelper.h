#pragma once

struct InstanceInfo {
	SUComponentInstanceRef instance;
	Transform transform;

	
	string modelName;
	string modelId;

	string type;
	map<string,string> attributes;
	string value;

};

class SketchupHelper
{
	
	static bool parseInstanceName(const string& name, /*out*/ InstanceInfo& meta);

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

	static void getInstancesRecursive(SUEntitiesRef, /*out*/ vector<InstanceInfo>& results, Transform = BaseTransform);
    
    static map<string,SUComponentDefinitionRef> getComponents(const vector<InstanceInfo>&);

	static SUModelRef openFile(const string& filename);
	
	
};
