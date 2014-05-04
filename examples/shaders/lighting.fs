varying vec3 iPosition; //interpolated vertex position (note: not multiplied with model matrix, position is relative to object/model and not the room)
varying vec3 iNormal; //interpolated normal 

varying vec3 iPositionWorld; //interpolated vertex position (note: not multiplied with model matrix, position is relative to object/model and not the room)
varying vec3 iNormalWorld; //interpolated normal 
uniform vec3 iPlayerPosition; //the player's position in the room

uniform sampler2D texDiffuse;

uniform float iGlobalTime;

int g_Debug = 1;

vec3 ambientLight = vec3(0,0,0);
vec3 diffuseLight = vec3(0,0,0);
vec3 specularLight = vec3(0,0,0);

//Prototye for generated function
void applySceneLights();


// Util functions
float flicker(float offset) {
	float t = offset + (iGlobalTime * 30.0);
	return sin(t*0.2) * cos(t * 0.3) * sin(t * 0.4) ;
}

vec3 flickerPos(vec3 pos) {
	float f=  flicker( pos.x+pos.y) * 0.2;
	return pos + vec3(f,f,f);
}


/*
	Apply a point light base on the inverse square law
	
	pos:	Position of the light
	colour: Colour and intensity
	near: Size of the light source, 0 for a point light
	range: How far to apply the light, saves GPU cycles

*/
void pointlight(vec3 pos, vec3 colour, float near, float range) {
	vec3 lightDir = pos - iPositionWorld;
	float dist = abs(length(lightDir));
	

	if(dist > range) {
		return; //Out of range
	}

	float i = 1.0 / (dist*dist);
	
	float NdotL = dot(iNormalWorld, lightDir);
	if(NdotL > 0.0)
		diffuseLight += i * colour * NdotL;

	vec3 rVector = normalize(2.0 * iNormalWorld * dot(iNormalWorld, lightDir) - lightDir);
	vec3 viewVector =  normalize(iPositionWorld-iPlayerPosition);
	float RdotV = dot(rVector, viewVector);
	
	if (RdotV > 0.0)
		specularLight = i * colour * 4.0 * pow(RdotV, 20.0);
}

/*
	A point light but wiht a flickery effect
	
*/
void torchlight(vec3 pos, vec3 colour, float near, float range) {
	pointlight(flickerPos(pos),colour, near, range); 
}

/*
	Set the global ambient level
*/
void ambient(vec3 pos, vec3 colour, float near, float range) {
	ambientLight = colour;
}

/* 
	Project a light pout of the players eyes
*/
void flashlight(vec3 colour) {
	
	vec4 p = gl_ModelViewProjectionMatrix * vec4(iPosition,1.0);
	
	float r = length(p.xy);
	float theta = atan(r, p.z);

	float i = 1.0 - theta * 2.0;
	
	if(i> 0.0){
		diffuseLight += (colour * i) / (p.z + 2.0);
	}
}

void main()
{
	applySceneLights();
    
    vec4 ka = gl_FrontMaterial.ambient;
	
	vec4 kd = gl_FrontMaterial.diffuse;
	if( gl_TexCoord[0].x != 9999.0 && gl_TexCoord[0].y != 9999.0) {
		kd *= texture2D(texDiffuse, gl_TexCoord[0].st);
	}

	//vec4 ks = gl_FrontMaterial.specular;
	vec4 ks = vec4(1,1,1,1);
	gl_FragColor =
		(ka + vec4(ambientLight,1.0)) +
		(kd * vec4(diffuseLight,1.0)) + 
		(ks * vec4(specularLight,1.0));
	
	if(g_Debug > 0) {
		if(mod(iPositionWorld.x+0.005,1.0) < 0.01 ||  mod(iPositionWorld.y+0.005,1.0) < 0.01 || mod(iPositionWorld.z+0.005,1.0) < 0.01) {
			gl_FragColor = vec4(1,1,0,1);
		}
	}
}
