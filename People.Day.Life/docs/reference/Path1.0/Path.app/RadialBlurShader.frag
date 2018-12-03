precision mediump float;
varying vec2 v_texCoord;
uniform sampler2D s_imageMap;
uniform sampler2D s_colorRamp;
uniform float outerVignetteDistance;
uniform float innerVignetteDistance;

uniform float sampleDist;
uniform float sampleStrength;

void main(void)
{
    float samples[10];
    samples[0] = -0.08;
    samples[1] = -0.05;
    samples[2] = -0.03;
    samples[3] = -0.02;
    samples[4] = -0.01;
    samples[5] =  0.01;
    samples[6] =  0.02;
    samples[7] =  0.03;
    samples[8] =  0.05;
    samples[9] =  0.08;

    vec2 dir = 0.5 - v_texCoord; 
    float dist = sqrt(dir.x*dir.x + dir.y*dir.y); 
    dir = dir/dist; 

    vec4 color = texture2D(s_imageMap, v_texCoord); 
    vec4 sum = color;

    for (int i = 0; i < 10; i++)
        sum += texture2D(s_imageMap, v_texCoord + dir * samples[i] * sampleDist);

    sum *= 1.0/11.0;
    float t = dist * sampleStrength;
    t = clamp( t ,0.0,1.0);

    vec4 result = mix( color, sum, t );
	
	// Vignette effect
	// if dist > outer, then 0 -> black 
	// if dist < inner, then 1 -> actual color
	// range of (outer - inner) is where the vignette occurs
	result *= smoothstep(outerVignetteDistance, innerVignetteDistance, dist);
	
	// Apply color ramp
	gl_FragColor = vec4(texture2D(s_colorRamp, vec2(result.r, 0)).r, 
						texture2D(s_colorRamp, vec2(result.g, 0)).g, 
						texture2D(s_colorRamp, vec2(result.b, 0)).b, 
						1.0);
}
