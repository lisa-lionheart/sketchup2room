#pragma once

struct InstanceInfo {
	SUComponentInstanceRef instance;
	Transform transform;

	
	string modelName;
	string modelId;

	string type;

	map<string,string> attributes; 
	string value;

	SUPoint3D offset;
    SUTextureRef texture;

	bool hasAttribute(const string& name) const {
		return attributes.find(name) != attributes.end();
	}

	string getAttribute(const string& name, const string& def = "") const {
		map<string,string>::const_iterator itr = attributes.find(name);
		if(itr == attributes.end()){
			return def;
		}
		return itr->second;
	}
};

class SketchupHelper
{
    SUModelRef m_Model;
    vector<InstanceInfo> m_Instances;
    
	map<string,SUComponentDefinitionRef> m_Components;
	map<string,string> m_Placeholders;

	SUEntitiesRef m_TopLevelEnts;
	
	bool parseInstanceName(const string& name, /*out*/ InstanceInfo& meta);
    
	void getInstancesRecursive(SUEntitiesRef ents, Transform = BaseTransform);
	void extractInstances(SUEntitiesRef ents, const Transform& parentTransform);
	void extractImages(SUEntitiesRef ents, const Transform& parentTransform);
    
	string extractImageToFile(SUImageRef);

public:
    SketchupHelper();
    
	bool openFile(const string& filename);
    
    vector<InstanceInfo>& instances(){ return m_Instances; }
    map<string,SUComponentDefinitionRef>& components(){ return m_Components; }
    map<string,string>& placeholders(){ return m_Placeholders; }
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
