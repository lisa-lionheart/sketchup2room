
#pragma once

#include "SketchupHelper.h"

class ShaderWriter {


	ofstream m_Shader;

public:
	ShaderWriter(const string& outFile);
	void writeLights(const vector<InstanceInfo>& instances);
	void includeFile(string filename, string fromFile = "");
};

