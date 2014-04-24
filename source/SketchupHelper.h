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
    SUModelRef m_Model;
    vector<InstanceInfo> m_Instances;
    map<string,SUComponentDefinitionRef> m_Components;
    SUEntitiesRef m_TopLevelEnts;
	
	bool parseInstanceName(const string& name, /*out*/ InstanceInfo& meta);
    
	void getInstancesRecursive(SUEntitiesRef ents, Transform = BaseTransform);
    void getComponents();
    
public:
    SketchupHelper();
    
	bool openFile(const string& filename);
    
    vector<InstanceInfo>& instances(){ return m_Instances; }
    map<string,SUComponentDefinitionRef>& components(){ return m_Components; }
    SUEntitiesRef topLevelEntities(){ return m_TopLevelEnts; }
    
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


	
	
};
