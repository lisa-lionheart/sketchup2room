varying vec3 iPosition; //interpolated vertex position (note: not multiplied with model matrix, position is relative to object/model and not the room)
varying vec3 iNormal; //interpolated normal 

varying vec3 iPositionWorld; //interpolated vertex position (note: not multiplied with model matrix, position is relative to object/model and not the room)
varying vec3 iNormalWorld; //interpolated normal 


varying float iTime;

uniform float frameTime;

void main()
{
	gl_FrontColor = gl_Color;
	gl_TexCoord[0] = gl_MultiTexCoord0;
	

	iPosition = gl_Vertex.xyz;
	iPositionWorld = gl_Vertex.xyz;

	gl_Position = ftransform();

	iTime = frameTime;
	iNormal = gl_Normal;
	iNormalWorld = gl_Normal;
}
