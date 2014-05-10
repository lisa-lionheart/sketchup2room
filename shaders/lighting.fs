

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
	
	if(dist > range) {
		return; //Out of range
	}

	float tangent = dot(lightDir,n);
	if(tangent > 0.0f) {
		dist -= 1.0f;
		dist -= near;

		float i = clamp(1.0f / (dist*dist), 0.0f, 1.0f);
		diffuseLight += i * tangent * colour;
	}

	vec3 rVector = normalize(2.0 * n * dot(n, lightDir) - lightDir);
	vec3 viewVector = -normalize(iPositionWorld-iPlayerPosition);
	float RdotV = dot(rVector, viewVector);
	
	if (RdotV > 0.0)
		specularLight += 0.1 * colour * pow(RdotV, 100.0) ;
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


