#version 330

uniform int u_genericDebugInt;
uniform float u_genericDebugFloat;
uniform float u_currentTime;
uniform vec3 u_lightWorldPosition;
uniform vec3 u_cameraWorldPosition;
uniform int u_useFog;
uniform float u_fogStart;
uniform float u_fogEnd;
uniform vec4 u_fogColor;

in vec2 v_texcoord;
in vec4 v_color;
in vec4 v_worldPosition;
in vec4 v_screenPosition;
in vec3 v_worldNormal;
in vec3 v_bitangent;
in vec3 v_tangent;

vec3 g_worldUnitNormal;
vec3 g_lightPosition;

out vec4 o_fragColor;

void main()
{

	g_lightPosition = u_lightWorldPosition;

	g_worldUnitNormal=normalize( v_worldNormal );
	vec3 pixelToLight=( g_lightPosition - v_worldPosition.xyz );
	pixelToLight=normalize( pixelToLight );
	float lightIntensity=dot( g_worldUnitNormal, pixelToLight );
	lightIntensity=clamp( lightIntensity, 0.0, 1.0 );
	vec4 litSurfaceColor=v_color;
	litSurfaceColor.rgb *= lightIntensity;
	
	o_fragColor = litSurfaceColor;
}