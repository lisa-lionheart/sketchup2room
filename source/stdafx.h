// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <stdio.h>

#ifdef WIN32
#include <tchar.h>
#include <Windows.h>
#else
#include <math.h>
#endif

#include <assert.h>

#include <slapi/slapi.h>
#include <slapi/geometry.h>
#include <slapi/initialize.h>
#include <slapi/unicodestring.h>
#include <slapi/model/model.h>
#include <slapi/model/entities.h>
#include <slapi/model/face.h>
#include <slapi/model/edge.h>
#include <slapi/model/mesh_helper.h>
#include <slapi/model/material.h>
#include <slapi/model/texture.h>
#include <slapi/model/mesh_helper.h>
#include <slapi/model/component_instance.h>
#include <slapi/model/component_definition.h>
#include <slapi/model/group.h>


#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;


#include "FileFunctions.h"
#include "Geometry.hpp"

#define ASSERT(expr) assert(expr)