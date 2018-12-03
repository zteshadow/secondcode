precision mediump float;
varying vec2 v_texCoord;
uniform sampler2D s_imageMap;
uniform sampler2D s_contrastMap;
uniform sampler2D s_overlayMap;

uniform sampler2D s_noiseMap;
uniform vec2 randomCoord1;
uniform vec2 randomCoord2;
uniform float frameCounter;

const float noiseFactor = 0.05;
const float scratchFactor = .0044;
const float scratchFactorInverse = 1.0 / scratchFactor;

const vec3 lumcoeff = vec3(0.299, 0.587, 0.114);
const vec3 sepiatone = vec3(0.9, 0.8, 0.6);

void main()
{
	// get color for current pixel of video frame
  	vec4 color = texture2D(s_imageMap, v_texCoord);
	
	// add scratch
	vec2 scratch = frameCounter * vec2(0.001, 0.4);
    scratch.x = fract(v_texCoord.x + scratch.x);
    float scratchValue = 2.0 * (texture2D(s_noiseMap, scratch).r * scratchFactorInverse);
	scratchValue = 1.0 - abs(1.0 - scratchValue);
	scratchValue = max(0.0, scratchValue);
    color.rgb += scratchValue; 
	
	// Calculate random coord + sample
    vec2 rCoord = (v_texCoord + randomCoord1 + randomCoord2) * 0.33;
    vec4 rand = texture2D(s_noiseMap, rCoord);
    if(noiseFactor > rand.r)
    {
        color.rgb = vec3(0.1 + rand.b * 0.4);
    }
	
	// Convert to grayscale
    float gray = dot(color.rgb, lumcoeff); 
	
	// To sepiatone
    color = vec4(gray * sepiatone, 1.0); // Sepia original (0.9, 0.7, 0.3)

    // Calc distance to center
    vec2 dist = 0.5 - v_texCoord;   
    // Random light fluctuation
    float fluc = randomCoord2.x * 0.04 - 0.02;
    // Vignette effect
    color.rgb *= (0.4 + fluc - dot(dist, dist))  * 2.8;
	
	gl_FragColor = color;
}