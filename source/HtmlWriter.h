#pragma once
#include "SketchupHelper.h"
#include <set>

class HtmlWriter
{
	string m_Title;
	string m_FileName;
	string m_DefaultShader, m_DefaultShaderId;

	ofstream m_Html;
	vector<string> m_Components;
    
    set<string> m_Assets;
	
	Transform m_Origin;

	string m_OutputDir;

	void writeAssets(const vector<InstanceInfo>& instances);
	void writeObject(const InstanceInfo& obj);

public:
	HtmlWriter(const string& outDir, const string& filename);
    
    void addAsset(const string& assetTag);

	void setDefaultShader(const string& s);
	
	bool write(const vector<InstanceInfo>& model);
	
	~HtmlWriter(void);
};

