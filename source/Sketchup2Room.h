
#pragma once

#include "Config.h"
#include "SketchupHelper.h"
#include "HtmlWriter.h"
#include "ModelWriter.h"
#include "ShaderWriter.h"

class SketchUp2Room
{
	Config m_Config;

	SketchupHelper m_Helper;
	HtmlWriter m_HtmlWriter;

	set<string> m_AssetsToCopy;
    

public:
	SketchUp2Room();
	bool parseArguments(int argc, char** argv);

	void readFile();

	void saveExternals();
	void writeGeometry();
	void writeShaders();
	void writeHtml();
};