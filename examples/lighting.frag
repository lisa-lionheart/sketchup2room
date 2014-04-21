varying vec3 iPosition; //interpolated vertex position (note: not multiplied with model matrix, position is relative to object/model and not the room)
varying vec3 iNormal; //interpolated normal 

uniform gl_MaterialParameters gl_FrontMaterial;
uniform sampler2D texDiffuse;

int g_Debug = 0;

vec3 light = vec3(0,0,0);

//Prototye for generated function
void applySceneLights();

/*
	Apply a point light base on the inverse square law
	
	pos:	Position of the light
	colour: Colour and intensity
	near: Size of the ight source, 0 for a point light
	range: How far to apply the light, saves GPU cycles

*/
void applyPointLight(vec3 pos, vec3 colour, float near, float range) {
	vec3 vec = pos - iPosition;
	float dist = abs(length(vec));
	
	if(dist > range) {
		return; //Out of range
	}
	
	float tangent = dot(vec/dist,iNormal);
	if(tangent < 0.0f) {
		return;	//Not facing light
	}
	
	dist -= near;
	
	float i = (1.0f / (dist*dist));
	light += i * tangent * colour;
}

void applyFlashLight(vec3 colour) {
	
	vec3 p = gl_ModelViewProjectionMatrix * vec4(iPosition,1.0);
	
	float r = length(p.xy);
	float theta = atan(r, p.z);

	float i = 1.0f - theta* 2;
	
	if(i> 0.0f){
		light += (colour * i) / (p.z+2);
	}
}

void main()
{
	applySceneLights();
	
	vec4 diffuse = gl_FrontMaterial.diffuse;
	diffuse *= texture2D(texDiffuse, gl_TexCoord[0].st);
	gl_FragColor = diffuse *  vec4(light,1.0);
	
	if(g_Debug) {
		if(mod(iPosition.x+0.005,1.0) < 0.01 ||  mod(iPosition.y+0.005,1.0) < 0.01 || mod(iPosition.z+0.005,1.0) < 0.01) {
			gl_FragColor = vec4(1,1,0,1);
		}
	}
}

//This will be generated
void applySceneLights() {
	applyFlashLight(  vec3(5.0, 5.0, 5.0) );

//	applyPointLight( vec3(2.0, 2.0, 2.0), vec3(5.0, 0.0, 0.0), 0.0f,4.0f);
}