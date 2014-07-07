#pragma once
#include "Config.h"
#include "SketchupHelper.h"
#include <set>

class HtmlWriter
{
	const Config& m_Config;

	
	Transform m_Origin;
	string m_Title;
	string m_DefaultShader, m_DefaultShaderId;


	ofstream m_Output;
	
	vector<string> m_Components;
    set<string> m_Assets;
	    
    map<string,string> m_RoomAtributes;
	
	void writeInstance(const InstanceInfo& obj);

	void writeObjectTag(const InstanceInfo& obj);
	void writeLinkTag(const InstanceInfo& obj);
	void writeTextTag(const InstanceInfo& obj);
	void writeImageTag(const InstanceInfo& obj);
	void writeSoundTag(const InstanceInfo& obj);
	void writeVideoTag(const InstanceInfo& obj);

	void writeAssets(const vector<InstanceInfo>& instances);
	void writeRoomTag(const vector<InstanceInfo>& instances);

public:
	HtmlWriter(const Config& config);
    
    void setRoomAtributes(const string& key, const string& value){ m_RoomAtributes[key] = value; }
    
    void addAsset(const string& assetTag);

	void setDefaultShader(const string& s);
	
	bool write(const vector<InstanceInfo>& model);
	
	~HtmlWriter(void);
};

