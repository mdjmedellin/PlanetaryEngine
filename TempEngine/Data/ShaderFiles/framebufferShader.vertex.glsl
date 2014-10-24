#version 330

uniform int u_genericDebugInt;
uniform float u_currentTime;
uniform mat4 u_mvpMatrix;
uniform vec2 u_renderWindowDimensions;
uniform sampler2D u_colorTexture;
uniform sampler2D u_depthTexture;
uniform sampler2D u_noiseTex;

in vec3 a_position;
in vec2 a_texCoord;

out vec2 v_texCoord;
out vec4 v_screenPosition;
out vec4 v_worldPosition;

void main()
{
	v_worldPosition = vec4( a_position, 1.0 );
	gl_Position = u_mvpMatrix * v_worldPosition; //u_mvpMatrix * v_worldPosition;
	v_screenPosition = gl_Position;
	v_texCoord = a_texCoord;
}