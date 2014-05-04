#pragma once


class Material
{
	

public:
	Material(SUMaterialRef);
	~Material(void);

	SUColor ambient;
	SUColor diffuse;
	SUColor specular;

	string diffuseTextureName;

		


};

