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