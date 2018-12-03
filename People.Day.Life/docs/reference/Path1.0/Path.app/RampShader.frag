precision mediump float;
varying vec2 v_texCoord;
uniform sampler2D s_imageMap;
uniform sampler2D s_colorRamp;
uniform float outerVignetteDistance;
uniform float innerVignetteDistance;

void main(void)
{
    vec4 color = texture2D(s_imageMap, v_texCoord); 
	
	// Vignette effect
	float d = distance(v_texCoord, vec2(0.5,0.5));
	// if d > outer, then 0 -> black 
	// if d < inner, then 1 -> actual color
	// range of (outer - inner) is where the vignette occurs
	color *= smoothstep(outerVignetteDistance, innerVignetteDistance, d);
	
	// Apply color ramp
	gl_FragColor = vec4(texture2D(s_colorRamp, vec2(color.r, 0)).r, 
				texture2D(s_colorRamp, vec2(color.g, 0)).g, 
				texture2D(s_colorRamp, vec2(color.b, 0)).b, 
				1.0);
}	

