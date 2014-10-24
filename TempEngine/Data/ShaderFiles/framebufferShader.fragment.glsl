#version 330

uniform int u_genericDebugInt;
uniform float u_currentTime;
uniform mat4 u_mvpMatrix;
uniform vec2 u_renderWindowDimensions;
uniform sampler2D u_colorTexture;
uniform sampler2D u_depthTexture;
uniform sampler2D u_noiseTex;

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
	//for testing purposes lets just render it with the default texture
	//o_fragColor = vec4( 0.0, 1.0, 0.0, 1.0 );
	o_fragColor = texture2D( u_colorTexture, v_texCoord );
	return;

	//Edge outline by the use of the depth image
	if( u_genericDebugInt == 0 )
	{
		int checkRadius = 5;
		float sampleTexCoordOffsetX = 0.001;
		float sampleTexCoordOffsetY = 0.001;
		float minDepth = 10000.0;
		float maxDepth = 0.0;
		float depthThreshold = 0.01;

		for( int yOffset = -checkRadius; yOffset <= checkRadius; ++yOffset )
		{
			for( int xOffset = -checkRadius; xOffset <= checkRadius; ++xOffset )
			{
				g_texCoords = v_texCoord + vec2( sampleTexCoordOffsetX * float( xOffset ), sampleTexCoordOffsetY * float( yOffset ) );
				vec4 depthBufferTexel = texture2D( u_depthTexture, g_texCoords );

				minDepth = min( minDepth, depthBufferTexel.r );
				maxDepth = max( maxDepth, depthBufferTexel.r );
			}
		}

		if( maxDepth - minDepth > depthThreshold )
		{
			o_fragColor = vec4( 0.0, 0.0, 0.0, 1.0 );
		}
		else
		{
			o_fragColor = texture2D( u_depthTexture, v_texCoord );
		}
	}

	//water effect
	if( u_genericDebugInt == 1 )
	{
		g_texCoords = v_texCoord;
		g_texCoords.x += sin( g_texCoords.y * 8 + u_currentTime * 5 ) / 100;
		o_fragColor = texture2D( u_colorTexture, g_texCoords );
		o_fragColor.rgb *= vec3( 0.12, 0.5, .7 );
	}

	//nightVision
	if( u_genericDebugInt == 2 )
	{
		float colorAmplification = 4.0;
		vec2 noiseCoords;

		vec4 nightVisionColor;
		
		noiseCoords.x = 0.4*sin(u_currentTime*50.0);                                 
		noiseCoords.y = 0.4*cos(u_currentTime*50.0);                                 
		
		vec3 noiseTexelColor = texture2D( u_noiseTex, ( v_texCoord.st * 5 ) + noiseCoords ).rgb;
		vec3 colorImageTexelColor = texture2D( u_colorTexture, v_texCoord.st + ( noiseTexelColor.xy * 0.005 ) ).rgb;
		
		colorImageTexelColor *= colorAmplification;
		
		//night vision is primarily green
		vec3 visionColor = vec3(0.01, 0.95, 0.01);
		nightVisionColor.rgb = ( colorImageTexelColor + ( noiseTexelColor * 0.2 ) ) * visionColor;
		o_fragColor.rgb = nightVisionColor.rgb;
		o_fragColor.a = 1.0;
    }
	
	//static tv effect, weird
	if( u_genericDebugInt == 3 )
	{
		float sinValue = sin( u_currentTime + v_screenPosition.y ) + 1.0;
		sinValue = clamp( sinValue, 0.0, 1.0 );
		o_fragColor = texture2D( u_colorTexture, v_texCoord );
		o_fragColor.rgb *= vec3( 1.0, 1.0, 1.0 ) * floor( sinValue );
		o_fragColor.a = 1.0;
	}

	//Static effect
	if( u_genericDebugInt == 4 )
	{
		g_texCoords = v_texCoord;
		float xOffset = 0.01;
		float cosValue = .5 * cos( v_worldPosition.x / 5 + u_currentTime * 4 ) + .5;
		float sinValue = sin( v_worldPosition.y / 5 + u_currentTime * 4 ) + 1.0;
		g_texCoords.y += cosValue * xOffset * floor( sinValue ); 
		o_fragColor = texture2D( u_colorTexture, g_texCoords );
		o_fragColor.rgb -= vec3( .2, .2, .2 ) * ( 1.0 - floor( sinValue ) );
		o_fragColor.a = 1.0;
	}

	//radar attempt
	if( u_genericDebugInt == 5 )
	{
		vec4 radarColor;
		float minDepth = .80;
		float currentPercentage = mod( u_currentTime, 4 ) - 2;
		currentPercentage *= .5;
		
		float emmiterToFragmentLength = length( vec2( 0.5, 0.5 ) - v_texCoord );
		
		if( emmiterToFragmentLength < currentPercentage )
		{
			g_texCoords = v_texCoord;
			vec4 depthBufferTexel = texture2D( u_depthTexture, g_texCoords );

			if( depthBufferTexel.r > minDepth )
			{
				o_fragColor = vec4( 0.0, 1.0, 0.0, 1.0 );
				o_fragColor.a *= clamp( ( depthBufferTexel.r - minDepth ) / minDepth, 0.0, 1.0 );

				o_fragColor.g *= currentPercentage / 1.0;
			}
		}
		if( emmiterToFragmentLength < ( currentPercentage + .01) && emmiterToFragmentLength > (currentPercentage - 0.01 ) )
		{
			o_fragColor.rgb = vec3( 0.0, 1.0, 0.0 );
		}
		if( emmiterToFragmentLength > currentPercentage )
		{
			currentPercentage = -( mod( u_currentTime, 4 ) - 2 );
			float radarIntensity = clamp( currentPercentage, 0.0, 1.0 );

			g_texCoords = v_texCoord;
			vec4 depthBufferTexel = texture2D( u_depthTexture, g_texCoords );

			o_fragColor = vec4( 0.0, 1.0, 0.0, 1.0 );
			o_fragColor.a *= clamp( ( depthBufferTexel.r - minDepth ) / minDepth, 0.0, 1.0 ) * currentPercentage / 1.0;

			o_fragColor.g *= currentPercentage / 1.0;
		}
	}
}