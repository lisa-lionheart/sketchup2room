
#pragma once

#include "SketchupHelper.h"

class ShaderWriter {


	ofstream m_Shader;

public:
	ShaderWriter(const string& outFile, const string& inFile);
	void writeLights(const vector<InstanceInfo>& instances);
};

