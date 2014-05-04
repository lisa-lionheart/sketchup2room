
int g_Debug;
vec3 ambientLight;

void torchlight(vec3 pos, vec3 colour, float near, float range);
void pointlight(vec3 pos, vec3 colour, float near, float range);

void applySceneLights() {
	ambientLight = vec3(0.1,0.1,0.1);

	torchlight(vec3(4,4,4), vec3(4.0,4.0,4.0), 0.0, 30.0);
}