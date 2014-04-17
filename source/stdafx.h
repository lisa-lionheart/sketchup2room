// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

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

#include <vector>
#include <iostream>
using namespace std;

#define ASSERT(expr) assert(expr)