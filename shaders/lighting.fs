varying vec3 iPosition; //interpolated vertex position (note: not multiplied with model matrix, position is relative to object/model and not the room)
varying vec3 iNormal; //interpolated normal 

varying vec3 iPositionWorld; //interpolated vertex position (note: not multiplied with model matrix, position is relative to object/model and not the room)
varying vec3 iNormalWorld; //interpolated normal 
uniform vec3 iPlayerPosition; //the player's position in the room

uniform int iUseTexture0; //use texture (0 - no, 1 - yes)?
uniform sampler2D iTexture0; //sampler to use for texture0

uniform sampler2D texDiffuse;

uniform float iGlobalTime;
uniform int iUseClipPlane; //use clip plane (0 - no, 1 - yes)?  (i.e. is the room viewed through a portal)
uniform vec4 iClipPlane; //equation of clip plane (xyz are normal, w is the offset, room is on side facing normal)


const float deg2rad = 57.29;

vec3 ambientLight = vec3(0.5,0.5,0.5);	//Total acumulated ambient light for pixel
vec3 diffuseLight = vec3(0,0,0);	//Total acumulated diffuse light for pixel
vec3 specularLight = vec3(0,0,0);	//Total acumulated specular light for pixel

//Prototype for generated function
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

	vec3 n = normalize(iNormalWorld);
	vec3 lightDir = normalize(pos - iPositionWorld);
	float dist = abs(length(pos - iPositionWorld));
	
	//float range = length(colour);

	if(dist > range) {
		return; //Out of range
	}

	float tangent = dot(lightDir,n);
	if(tangent > 0.0) {
		dist -= 1.0;
		dist -= near;

		float i = clamp(1.0 / (dist*dist), 0.0, 1.0);
		diffuseLight += i * tangent * colour;
	}

	vec3 rVector = normalize(2.0 * n * dot(n, lightDir) - lightDir);
	vec3 viewVector = -normalize(iPositionWorld-iPlayerPosition);
	float RdotV = dot(rVector, viewVector);
	
	if (RdotV > 0.0)
		specularLight += 0.1 * colour * pow(RdotV, 100.0) ;
}

/*
	Spot light, casts a directed cone of light from the given point in a set direction.

	pos: 	   The world position of the light
	direction:  Direction to cast the light
	outerCone:  The angle of the maximum extent of the light 
	innerCone:  Defines the fall off of light to the oute cone, setting them to the same value will result in
	 (unrealistic) a sharp edge. 5-15 degree seperation looks quite nice in most situations
*/
void spotlight(vec3 pos, vec3 direction, vec3 col, float outerCone, float innerCone, float range){
	vec3 n = normalize(iNormalWorld);
	vec3 lightDir = normalize(pos - iPositionWorld);
	float dist = abs(length(pos - iPositionWorld));
	
	if(dist > range) {
		return; //Out of range
	}

	float theta = deg2rad * acos(dot(direction, -lightDir));
	if(theta > outerCone){	//Outside of light cone
		return;
	}

	float tangent = dot(lightDir,n);
	if(tangent < 0.0) {
		return;
	}

	float i = 1.0 - (theta - innerCone) / (outerCone-innerCone);

	i= i / (dist*dist);

	diffuseLight += i * tangent * col;

	vec3 rVector = normalize(2.0 * n * dot(n, lightDir) - lightDir);
	vec3 viewVector = -normalize(iPositionWorld-iPlayerPosition);
	float RdotV = dot(rVector, viewVector);
	
	if (RdotV > 0.0)
		specularLight += 0.1 * col * pow(RdotV, 100.0) ;

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
	Project a light point out of the players eyes
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


