#version 330

uniform int u_genericDebugInt;
uniform float u_genericDebugFloat;
uniform float u_currentTime;
uniform mat4 u_mvpMatrix;
uniform vec3 u_lightWorldPosition;
uniform vec3 u_cameraWorldPosition;

in vec3 a_worldPosition;
in vec2 a_texCoords;
in vec4 a_color;
in vec3 a_normal;
in vec3 a_bitangent;
in vec3 a_tangent;

out vec2 v_texcoord;
out vec4 v_color;
out vec4 v_worldPosition;
out vec4 v_screenPosition;
out vec3 v_worldNormal;
out vec3 v_bitangent;
out vec3 v_tangent;

void main()
{
   v_worldPosition = vec4 ( a_worldPosition, 1.0 );
   
   if( u_genericDebugInt == 4 )
	{
		v_worldPosition.y += .05 * sin( u_currentTime * 7 + v_worldPosition.x ) + .05 * sin( u_currentTime * 13 + v_worldPosition.z );
		v_worldPosition.x += .05 * sin( u_currentTime * 11 + v_worldPosition.y ) + .05 * sin( u_currentTime * 17 + v_worldPosition.z );
	}

   gl_Position = u_mvpMatrix * v_worldPosition;
   v_screenPosition = gl_Position;
   v_texcoord = a_texCoords;
   v_color = a_color;
   v_worldNormal = a_normal;
   v_bitangent = a_bitangent;
   v_tangent = a_tangent;
}