#pragma once


class Material
{
	

public:
	Material(SUMaterialRef);
	~Material(void);

	SUColor ambient;
	SUColor diffuse;
	SUColor specular;

	double alpha;

	SUMaterialRef rawMaterial;

	string name;
	string fullName;
	string diffuseTextureName;


	void saveTextures(const string& dir);
		
	void write(ostream&) const;

};


inline ostream& operator<<(ostream& out, const Material& mat) {
	mat.write(out);
	return out;
}