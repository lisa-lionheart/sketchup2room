#pragma once

#include "stdafx.h"

#include <slapi/geometry.h>

union Transform {
	double values[16];
	double m[4][4];
	struct {
		double aa, ba, ca, da;
		double ab, bb, cb, db;
		double ac, bc, cc, dc;
		double ad, bd, cd, dd;
	};
};

extern const float g_Scale;
extern const Transform TransformIdentity;
extern const Transform BaseTransform;

extern const SUVector3D xaxis;
extern const SUVector3D yaxis;
extern const SUVector3D zaxis;

inline Transform operator*(const Transform& a, const Transform& b) {
	Transform r;
	int i,j;

    for (i=0; i<4; i++)
    {
		for (j=0; j<4; j++)
		{
			r.m[j][i] = a.m[0][i] * b.m[j][0] + a.m[1][i] * b.m[j][1] + a.m[2][i] * b.m[j][2] + a.m[3][i] * b.m[j][3];
		}
	}
	return r;
}

inline SUPoint3D operator*(const SUPoint3D& a, const Transform& t) {
	SUPoint3D r ={ 
		a.x * t.aa + a.y * t.ab + a.z * t.ac + t.ad,
		a.x * t.ba + a.y * t.bb + a.z * t.bc + t.bd,
		a.x * t.ca + a.y * t.cb + a.z * t.cc + t.cd
	};
	return r;
}

inline SUPoint3D operator*(const SUPoint3D& a, double b) {
	SUPoint3D c= { a.x * b, a.y * b, a.z * b};
	return c;
}

inline SUPoint3D operator/(const SUPoint3D& a, double b) {
	SUPoint3D c= { a.x / b, a.y / b, a.z / b};
	return c;
}


inline SUVector3D operator*(const SUVector3D& a, double b) {
	SUVector3D c= { a.x * b, a.y * b, a.z * b};
	return c;
}

inline SUVector3D operator/(const SUVector3D& a, double b) {
	SUVector3D c= { a.x / b, a.y / b, a.z / b};
	return c;
}

inline SUVector3D operator*(const SUVector3D& a, const Transform& t) {
	SUVector3D r = {
		a.x * t.aa + a.y * t.ab + a.z * t.ac,
		a.x * t.ba + a.y * t.bb + a.z * t.bc,
		a.x * t.ca + a.y * t.cb + a.z * t.cc
	};
	return r;
}

inline ostream& operator<<(ostream& html, const SUVector3D& v) {
	return html << v.x << " " << v.y << " " << v.z;
}

inline ostream& operator<<(ostream& html, const SUPoint3D& v) {
	return html << v.x << " " << v.y << " " << v.z;
}


inline double length(const SUVector3D& v) {
	return sqrtf((v.x*v.x) + (v.y*v.y) + (v.z*v.z));
}

inline ostream& writeTransform(ostream& html, Transform t, bool flipYZ = false){
	SUPoint3D pos = { 0 };

	html << "pos=\""  << (pos * t) / g_Scale << "\" "; 
	html << "xdir=\"" << xaxis*t << "\" "; 

	if(!flipYZ) {
		html << "ydir=\"" << yaxis*t << "\" "; 
		html << "zdir=\"" << zaxis*t << "\" "; 
	}else {
		html << "ydir=\"" << zaxis*t << "\" "; 
		html << "zdir=\"" << yaxis*t << "\" "; 
	}

	return html;
}


inline ostream& operator<<(ostream& html, Transform t) {
	return writeTransform(html,t);
}

inline SUVector3D getTransformScale(const Transform& t) {
	SUVector3D scale = { length(xaxis * t),   length(yaxis * t), length(zaxis * t) };
	return scale;
}

