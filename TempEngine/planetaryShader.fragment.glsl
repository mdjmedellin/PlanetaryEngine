#version 330

const int MAX_LIGHTS = 16;

uniform int u_genericDebugInt;
uniform float u_genericDebugFloat;
uniform float u_currentTime;
uniform sampler2D u_diffuseTexUniform;
uniform sampler2D u_normalTexUniform;
uniform sampler2D u_specularTexUniform;
uniform sampler2D u_emissiveTexUniform;
uniform sampler2D u_bumpTexUniform;
uniform sampler2D u_noiseTexUniform;
uniform vec4 u_fogColor;
uniform vec3 u_cameraWorldPosition;
uniform float u_fogStart;
uniform float u_fogEnd;
uniform float u_surfaceShininess;
uniform float u_useDiffuse;
uniform float u_useSurfaceColor;
uniform float u_useNormalMap;
uniform float u_useEmissive;
uniform float u_useBumpMap;
uniform float u_useSpecularMap;
uniform float u_useFog;
uniform float u_useLight;

//light uniforms
uniform vec3 u_lightsWorldPosition[ MAX_LIGHTS ];
uniform vec3 u_lightsPointColor[ MAX_LIGHTS ];
uniform vec3 u_lightsSpotColor[ MAX_LIGHTS ];
uniform vec3 u_lightsDirection[ MAX_LIGHTS ];
uniform vec3 u_lightsDirectionalColor[ MAX_LIGHTS ];
uniform float u_lightsSpotBrightness[ MAX_LIGHTS ];
uniform float u_lightsPointBrightness[ MAX_LIGHTS ];
uniform float u_lightsInnerRadii[ MAX_LIGHTS ];
uniform float u_lightsOuterRadii[ MAX_LIGHTS ];
uniform float u_lightsInnerDots[ MAX_LIGHTS ];
uniform float u_lightsOuterDots[ MAX_LIGHTS ];
uniform float u_lightsDirectionalBrightness[ MAX_LIGHTS ];
uniform int u_numberOfLights;

in vec2 v_texcoord;
in vec4 v_color;
in vec4 v_worldPosition;
in vec4 v_screenPosition;
in vec3 v_worldNormal;
in vec3 v_bitangent;
in vec3 v_tangent;

vec2 g_texCoords;
vec3 g_worldUnitNormal;
vec3 g_normal;
vec3 g_tangent;
vec3 g_bitangent;
vec3 g_lightPosition;
vec4 g_emissiveGlow;
vec4 g_litSurfaceColor;
vec3 g_specular;
float g_fogIntensity;

out vec4 o_fragColor;

//create a function that transforms a vector into color
void DebugVector( vec3 vector )
{
	//encode as rgb and hammer to the o_fragColor;
	o_fragColor = vec4( ( vector + 1.0 ) * 0.5, 1.0 );
}

float CalculateLuminanceForColor( const vec3 color )
{
	const vec3 luminanceWeightPerRGB = vec3( .2125, .7154, .0721 );
	return dot( color, luminanceWeightPerRGB );
}

void main()
{
	g_tangent = normalize( v_tangent );
	g_bitangent = normalize( v_bitangent );
	g_normal = normalize( v_worldNormal );

	mat3 tangentToWorld = mat3( g_tangent, g_bitangent, g_normal );
	mat3 worldToTangent = inverse( tangentToWorld );
	
	vec3 pointToCameraWorld = u_cameraWorldPosition - v_worldPosition.xyz;
	pointToCameraWorld = normalize( pointToCameraWorld );
	vec3 pointToCameraTangentSpace = worldToTangent * pointToCameraWorld;
	pointToCameraTangentSpace.y *= -1.0;

	//BUMP MAP
	g_texCoords = v_texcoord;
	
	vec4 bumpMapTexel = texture2D( u_bumpTexUniform, v_texcoord );
	float height = bumpMapTexel.r;
	g_texCoords += vec2( 0.0, 0.0 ) * ( 1.0 - u_useBumpMap ) + pointToCameraTangentSpace.xy * height * 0.03 * u_useBumpMap;

	//NORMAL
	//checking what should we use as our normal
	vec4 sampleNormalMapValue = texture2D( u_normalTexUniform, g_texCoords );
	vec3 worldNormalMapValue = tangentToWorld * ( ( sampleNormalMapValue.rgb * 2.0 ) - 1.0 );

	g_worldUnitNormal=normalize( v_worldNormal );
	g_worldUnitNormal = g_worldUnitNormal * ( 1.0 - u_useNormalMap ) + worldNormalMapValue * u_useNormalMap;
	g_worldUnitNormal = normalize( g_worldUnitNormal );

	//LIGHTS
	g_specular = vec3( 0.0, 0.0, 0.0 );
	vec4 totalDiffuseLight = vec4( 0.0, 0.0, 0.0, 1.0 );
	vec4 currentLightDiffuseLight = vec4( 0.0, 0.0, 0.0, 1.0 );
	vec3 pixelToLight = vec3( 0.0, 0.0, 0.0 );
	vec3 cameraToPointReflection = reflect( -pointToCameraWorld, g_worldUnitNormal );
	float distanceToLight = 0.0;
	float useCurrentLight = 0.0;

	for( int iterator = 0; iterator < MAX_LIGHTS; ++iterator )
	{
		useCurrentLight = u_numberOfLights - iterator;
		useCurrentLight = clamp( useCurrentLight, 0.0, 1.0 );

		currentLightDiffuseLight = vec4( 0.0, 0.0, 0.0, 1.0 );
		pixelToLight = ( u_lightsWorldPosition[ iterator ] - v_worldPosition.xyz );
		distanceToLight = length( pixelToLight );
		pixelToLight = normalize( pixelToLight );

		//calculate the current point light value
		float pointLightIntensity = u_lightsOuterRadii[ iterator ] - distanceToLight;
		pointLightIntensity = pointLightIntensity / ( u_lightsOuterRadii[ iterator ] - u_lightsInnerRadii[ iterator ] );
		pointLightIntensity = clamp( pointLightIntensity, 0.0, 1.0 );
		
		float outerRadiusIntensity = smoothstep( 0.0, 1.0, pointLightIntensity );
		float innerRadiusIntensity = floor( pointLightIntensity );
		float radialIntensity = innerRadiusIntensity + ( 1.0 - innerRadiusIntensity ) * ( outerRadiusIntensity );

		vec3 lightFromCurrentPointLight = u_lightsPointColor[ iterator ] * radialIntensity;

		currentLightDiffuseLight.rgb += lightFromCurrentPointLight * u_lightsPointBrightness[ iterator ];
		
		//calculate the current spot light value
		vec3 lightDirection = normalize( u_lightsDirection[ iterator ] );
		float pixelToLightDirectionDot = dot( pixelToLight, -lightDirection );
		float spotLightIntensity = pixelToLightDirectionDot - u_lightsOuterDots[ iterator ];
		spotLightIntensity = spotLightIntensity / ( u_lightsInnerDots[ iterator ] - u_lightsOuterDots[ iterator ] );
		spotLightIntensity = clamp( spotLightIntensity, 0.0, 1.0 );
		
		spotLightIntensity = smoothstep( 0.0, 1.0, spotLightIntensity );
		float apertureIntensity = floor( spotLightIntensity );
		float coneIntensity = ( apertureIntensity ) + ( 1.0 - apertureIntensity ) * ( spotLightIntensity );
		
		vec3 lightFromCurrentSpotLight = u_lightsSpotColor[ iterator ] * coneIntensity;

		currentLightDiffuseLight.rgb += lightFromCurrentSpotLight * u_lightsSpotBrightness[ iterator ] * radialIntensity;


		//determining the shadowing of the light according to the normals
		currentLightDiffuseLight.rgb *= clamp ( dot( pixelToLight, g_worldUnitNormal ), 0.0, 1.0 );
		totalDiffuseLight.rgb += currentLightDiffuseLight.rgb * useCurrentLight;

		//directional light
		float directionalLightIntensity = dot( g_worldUnitNormal, -lightDirection );
		directionalLightIntensity = clamp( directionalLightIntensity, 0.0, 1.0 );
		vec3 lightFromCurrentDirectionalLight = u_lightsDirectionalColor[ iterator ] * directionalLightIntensity;
		currentLightDiffuseLight.rgb += lightFromCurrentDirectionalLight;
		totalDiffuseLight.rgb += lightFromCurrentDirectionalLight * useCurrentLight;


		//specular intensity
		float specularIntensity = dot( pixelToLight, cameraToPointReflection );
		vec4 specularColor = vec4( 1.0, 1.0, 1.0, 1.0 ) * ( 1.0 - u_useSpecularMap ) + texture2D( u_specularTexUniform, g_texCoords ) * u_useSpecularMap;
		float shininess = max( specularColor.r, specularColor.g );
		shininess = max( shininess, specularColor.b );
		shininess = u_surfaceShininess * ( 1.0 - u_useSpecularMap ) + shininess * ( u_useSpecularMap );
		
		specularIntensity = clamp( specularIntensity, 0.001, 1.0 );
		specularIntensity = pow( specularIntensity, 32.0 * shininess );
		specularIntensity *= shininess;
		g_specular += ( currentLightDiffuseLight.rgb * specularColor.rgb ) * specularIntensity * ( 1.0 - floor( u_lightsDirectionalBrightness[ iterator ] ) ) * useCurrentLight;
	}

	vec4 directLight = vec4( vec3( 1.0, 1.0, 1.0 ) * ( 1.0 - u_useLight ) + totalDiffuseLight.rgb * ( u_useLight ), 1.0 );
	
	//should we use the diffuse color
	vec4 diffuseColor = vec4( 1.0, 1.0, 1.0, 1.0 ) * ( 1.0 - u_useDiffuse ) + texture2D( u_diffuseTexUniform, g_texCoords ) * ( u_useDiffuse );
	
	//should we use the surface color
	vec4 surfaceColor = vec4( 1.0, 1.0, 1.0, 1.0 ) * ( 1.0 - u_useSurfaceColor ) + v_color * u_useSurfaceColor;
	
	//FOG
	//calculating the fog intensity
	g_fogIntensity = 0.0;
	g_emissiveGlow = vec4( 0.0, 0.0, 0.0, 0.0 );

	if( u_fogEnd != u_fogStart )
	{
		g_fogIntensity = length( v_worldPosition.xyz - u_cameraWorldPosition ) - u_fogStart;
		g_fogIntensity = g_fogIntensity / ( u_fogEnd - u_fogStart );
		g_fogIntensity = clamp( g_fogIntensity, 0.0, 1.0 );
	}

	//EMMISIVE
	//calculating if we should use the emmisive
	g_emissiveGlow = vec4( 1.0, 1.0, 1.0, 1.0 );
	g_emissiveGlow.rgb = texture2D( u_emissiveTexUniform, g_texCoords ).rgb;
	g_emissiveGlow *= u_useEmissive;

	//SPECULAR INTENSITY
	/*
	vec3 lightToPointWorld = v_worldPosition.xyz - u_lightWorldPosition;
	lightToPointWorld = normalize( lightToPointWorld );

	vec3 reflection = reflect( lightToPointWorld, g_worldUnitNormal );
	float specularIntensity = dot( reflection, pointToCameraWorld );

	vec4 specularColor = vec4( 0.0, 1.0, 0.0, 1.0 ) * ( 1.0 - u_useSpecularMap ) + texture2D( u_specularTexUniform, g_texCoords ) * u_useSpecularMap;
	float shininess = max( specularColor.r, specularColor.g );
	shininess = max( shininess, specularColor.b );
	shininess = u_surfaceShininess * ( 1.0 - u_useSpecularMap ) + shininess * ( u_useSpecularMap );

	specularIntensity = clamp( specularIntensity, 0.001, 1.0 );
	specularIntensity = pow( specularIntensity, 32.0 * shininess );
	specularIntensity *= shininess;
	g_specular = u_lightColor.rgb * specularIntensity * specularColor.rgb;
	*/

	o_fragColor = diffuseColor * directLight * surfaceColor;

	
	//prototype frozen effect
	if( u_genericDebugInt == 1 )
	{
		o_fragColor += vec4( 0.0, .5, 1.0, .5 );
		float specularIntensity = dot( g_worldUnitNormal, cameraToPointReflection );
		float shininess = 1.0;
		specularIntensity = clamp( specularIntensity, 0.001, 1.0 );
		specularIntensity = pow( specularIntensity, 32.0 * shininess );
		specularIntensity *= shininess;
		g_specular = o_fragColor.rgb * specularIntensity * vec3( 0.0, .5, 1.0 );
	}

	
	//toon shader
	if( u_genericDebugInt == 2 )
	{
		vec4 toonShadingColor;
		float lightIntensity = max( totalDiffuseLight.r, totalDiffuseLight.g );
		lightIntensity = max( lightIntensity, totalDiffuseLight.b );

		if ( lightIntensity > 0.95)      toonShadingColor = vec4( 0.9,0.9,0.9,1.0);
		else if ( lightIntensity > 0.50) toonShadingColor = vec4(0.5,0.5,0.5,1.0);
		else                             toonShadingColor = vec4(0.1,0.1,0.1,1.0);

		o_fragColor *= toonShadingColor;
	}
	
	//o_fragColor.rgb += g_emissiveGlow.rgb;
	//o_fragColor.rgb += g_specular.rgb;

	//fire attempt
	if( u_genericDebugInt == 3 )
	{
		float relativeHeight = 0.5 + 0.5 * v_worldPosition.y;
        
        float relativeTime1 = ( 0.21 * u_currentTime ) - ( 0.11 * v_worldPosition.y ) - ( 0.02 * sin( -1.3 * u_currentTime + 3.7 * v_worldPosition.x )) + ( 0.09 * sin( 6.1 + 7.0 * v_worldPosition.z ));
        float relativeTime2 = ( 0.37 * u_currentTime ) - ( 0.23 * v_worldPosition.y ) - ( 0.02 * sin( 7.0 * u_currentTime + 13.0 * v_worldPosition.x )) + ( 0.01 * sin( 3.9 + 11.0 * v_worldPosition.z ));
        float relativeTime3 = ( 0.19 * u_currentTime ) - ( 0.17 * v_worldPosition.y ) - ( 0.02 * sin( -4.3 * u_currentTime + 8.1 * v_worldPosition.x )) + ( 0.04 * sin( 9.3 + 6.0 * v_worldPosition.z ));
        
        vec2 noiseTexCoords1 = vec2( relativeTime1, 0.1 );
        vec2 noiseTexCoords2 = vec2( relativeTime2, 0.2 );
        vec2 noiseTexCoords3 = vec2( relativeTime3, 0.3 );
        
        vec3 noiseTexel1 = texture2D( u_noiseTexUniform, noiseTexCoords1 ).rgb + .1;
        vec3 noiseTexel2 = texture2D( u_noiseTexUniform, noiseTexCoords2 ).rgb + .07;
        vec3 noiseTexel3 = texture2D( u_noiseTexUniform, noiseTexCoords3 ).rgb + .31;

        float noiseIntensity1 = noiseTexel1.r;
        float noiseIntensity2 = noiseTexel2.r;
        float noiseIntensity3 = noiseTexel3.r;

        float noise = ( noiseIntensity1 * noiseIntensity1 + noiseIntensity2 * noiseIntensity2 + noiseIntensity3 * noiseIntensity3 );
        
        float intensity = max( 0.0, ( 0.4 + noise ) * ( 1.0 - relativeHeight));
        float blue = intensity;
        float green = blue * blue * blue;
        float red = green * green * green;
        o_fragColor.rgb += vec3( red, green, blue );
	}
	
	o_fragColor.rgb += g_emissiveGlow.rgb;
	o_fragColor.rgb += g_specular.rgb;

	//fog is the last step
	o_fragColor = ( g_fogIntensity * u_useFog * u_fogColor ) + ( ( 1.0 - ( g_fogIntensity * u_useFog ) ) * o_fragColor );

	//DebugVector( currentAccumulatedLight.rgb );

	//DebugVector( g_worldUnitNormal );
}