#include janus_uniforms.fs
#include lighting.fs

int g_Debug = 0;

void main()
{
	//Clip plane check
	if (iUseClipPlane == 1 && dot(iPosition, iClipPlane.xyz) < iClipPlane.w) {
		discard;
		return;
	}
	
	vec4 ka = gl_FrontMaterial.ambient;
	vec4 kd = gl_FrontMaterial.diffuse;
	vec4 ks = gl_FrontMaterial.specular;
	
	if( iUseTexture0 == 1) {
		vec4 kdm = texture2D(iTexture0, gl_TexCoord[0].st);
		
		if(kdm.a > 0.1) {
			kdm.a = 1.0;
		}
		
		kd *= kdm;
	}

	if(kd.a < 0.1) {
		discard;
	}
	applySceneLights();
    
	vec3 col = kd.rgb * (diffuseLight + ambientLight + ka.rgb) + (ks.rgb * specularLight);
	gl_FragColor = vec4(col,kd.a);
	
	if(g_Debug > 0) {
		if(mod(iPositionWorld.x+0.005,1.0) < 0.01 ||  mod(iPositionWorld.y+0.005,1.0) < 0.01 || mod(iPositionWorld.z+0.005,1.0) < 0.01) {
			gl_FragColor = vec4(1,1,0,1);
		}
	}
}