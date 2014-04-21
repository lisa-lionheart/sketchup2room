#pragma once

struct InstanceMeta
{
	string type;
	map<string,string> attributes;
	string value; //Url
};


class HtmlWriter
{
	string m_Title;
	string m_FileName;
	string m_DefaultShader, m_DefaultShaderId;

	ofstream m_Html;
	vector<string> m_Components;
	
	Transform m_Origin;

	string m_OutputDir;

	void writeAssets(const vector<SUComponentInstanceRef>& instances);
	void writeObject(SUComponentInstanceRef obj);

	bool parseInstanceName(const string& name, /*out*/ InstanceMeta& meta);

public:
	HtmlWriter(const string& outDir, const string& filename);

	void setDefaultShader(const string& s){ 
		m_DefaultShader = s; 
		m_DefaultShaderId = "shader_id=\"default_shader_id\" ";
	}
	
	bool write(SUModelRef model);
	
	~HtmlWriter(void);
};

