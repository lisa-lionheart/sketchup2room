#pragma once
#include "SketchupHelper.h"


class HtmlWriter
{
	string m_Title;
	string m_FileName;
	string m_DefaultShader, m_DefaultShaderId;

	ofstream m_Html;
	vector<string> m_Components;
	
	Transform m_Origin;

	string m_OutputDir;

	void writeAssets(const vector<InstanceInfo>& instances);
	void writeObject(const InstanceInfo& obj);

public:
	HtmlWriter(const string& outDir, const string& filename);

	void setDefaultShader(const string& s){ 
		m_DefaultShader = s; 
		m_DefaultShaderId = "shader_id=\"default_shader_id\" ";
	}
	
	bool write(const vector<InstanceInfo>& model);
	
	~HtmlWriter(void);
};

