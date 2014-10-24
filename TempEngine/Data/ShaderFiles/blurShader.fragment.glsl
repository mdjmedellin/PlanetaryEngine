#version 330

uniform sampler2D u_colorTexture;
uniform float u_horizontalBlur;
uniform float u_verticalBlur;

in vec2 v_texCoord;
in vec4 v_screenPosition;
in vec4 v_worldPosition;

vec2 g_texCoords;

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

	float sampleTexCoordOffsetX = 0.001;
	float sampleTexCoordOffsetY = 0.001;

	float value[9];
	value[0]=0.05;
	value[1]=0.09;
	value[2]=0.11;
	value[3]=0.15;
	value[4]=0.20;
	value[5]=0.15;
	value[6]=0.11;
	value[7]=0.09;
	value[8]=0.05;
	
	vec4 resultingVerticalBlend = vec4( 0.0, 0.0, 0.0, 1.0 );
	vec4 resultingHorizontalBlend = vec4( 0.0, 0.0, 0.0, 1.0 );
	
	int blurRadius = 4;
	
	for( int yOffset = -blurRadius; yOffset <= blurRadius; ++yOffset )
	{
		vec2 texCoords = v_texCoord + vec2( 0.0, sampleTexCoordOffsetY * float( yOffset ) );
		resultingVerticalBlend.rgb += texture2D( u_colorTexture, texCoords ).rgb * value[ yOffset + blurRadius ];
	}
	for( int xOffset = -blurRadius; xOffset <= blurRadius; ++xOffset )
	{
		vec2 texCoords = v_texCoord + vec2( sampleTexCoordOffsetX * float( xOffset ), 0.0 );
		resultingHorizontalBlend.rgb += texture2D( u_colorTexture, texCoords ).rgb * value[ xOffset + blurRadius ];
	}
	
	o_fragColor = vec4( ( resultingVerticalBlend.rgb * u_verticalBlur + resultingHorizontalBlend.rgb * u_horizontalBlur ) * ( 1.0 / (u_verticalBlur + u_horizontalBlur) ), 1.0 );
}