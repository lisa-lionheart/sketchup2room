varying vec3 iPosition; //interpolated vertex position (note: not multiplied with model matrix, position is relative to object/model and not the room)
varying vec3 iNormal; //interpolated normal 

varying vec3 iPositionWorld; //interpolated vertex position (note: not multiplied with model matrix, position is relative to object/model and not the room)
varying vec3 iNormalWorld; //interpolated normal 
uniform vec3 iPlayerPosition; //the player's position in the room

uniform gl_MaterialParameters gl_FrontMaterial;
uniform sampler2D texDiffuse;

uniform float iGlobalTime;

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
void pointlight(vec3 pos, vec3 colour, float near, float range) {
	vec3 vec = pos - iPositionWorld;
	float dist = abs(length(vec));
	
	if(dist > range) {
		return; //Out of range
	}
	
	float tangent = dot(vec/dist,iNormalWorld/length(iNormalWorld));
	if(tangent < 0.0f) {
		return;	//Not facing light
	}
	
	dist -= 1.0f;
	dist -= near;
	
	float i = clamp(1.0f / (dist*dist), 0.0f, 1.0f);
	light += i * tangent * colour;
}

float flicker(float offset) {
	float t = offset + (iGlobalTime* 30);
	return sin(t*0.2) * cos(t * 0.3) * sin(t * 0.4) ;
}

vec3 flickerPos(vec3 pos) {
	float f=  flicker( pos.x+pos.y) * 0.2;
	return pos + vec3(f,f,f);
}

void torchlight(vec3 pos, vec3 colour, float near, float range) {
	pointlight(flickerPos(pos),colour, near, range); 
}


void ambient(vec3 pos, vec3 colour, float near, float range) {
	light += colour;
}

void flashlight(vec3 colour) {
	
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
	if( gl_TexCoord[0].x != 9999.0f && gl_TexCoord[0].y != 9999.0f) {
		diffuse *= texture2D(texDiffuse, gl_TexCoord[0].st);
	}
	
	gl_FragColor = diffuse *  vec4(light,1.0);
	
	if(g_Debug) {
		if(mod(iPositionWorld.x+0.005,1.0) < 0.01 ||  mod(iPositionWorld.y+0.005,1.0) < 0.01 || mod(iPositionWorld.z+0.005,1.0) < 0.01) {
			gl_FragColor = vec4(1,1,0,1);
		}
	}
}
