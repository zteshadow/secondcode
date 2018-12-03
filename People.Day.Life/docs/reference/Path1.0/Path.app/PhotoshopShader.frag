precision mediump float;
varying vec2 v_texCoord;
uniform sampler2D s_imageMap;
uniform sampler2D s_contrastMap;
uniform float hueAdjustment;
uniform float saturationMultiplier;
uniform float brightness;
uniform vec3 colorShift;
uniform float outerVignetteDistance;
uniform float innerVignetteDistance;

// From: http://beesbuzz.biz/code/hsv_color_transforms.php
// H - hue shift in radians
// S - saturation multiplier (scalar)
// V - value mulitplier (scalar)
vec4 TransformHSV(vec4 color, float H, float S, float V) {
    float VSU = V*S*cos(H);
    float VSW = V*S*sin(H);
	return vec4((0.299*V+0.701*VSU+0.168*VSW)*color.r + (0.587*V-0.587*VSU+0.330*VSW)*color.g + (0.114*V-0.114*VSU-0.497*VSW)*color.b,
		(0.299*V-0.299*VSU-0.328*VSW)*color.r + (0.587*V+0.413*VSU+0.035*VSW)*color.g + (0.114*V-0.114*VSU+.292*VSW)*color.b,
		(0.299*V-0.3*VSU+1.25*VSW)*color.r + (0.587*V-0.588*VSU-1.05*VSW)*color.g + (0.114*V+0.886*VSU-0.203*VSW)*color.b,
		1.0);
}

void main(void)
{
    vec4 color = texture2D(s_imageMap, v_texCoord); 
	color = TransformHSV(color, hueAdjustment, saturationMultiplier, 1.0);
	color = vec4(clamp(color.r + colorShift.r + brightness, 0.0, 1.0),
				clamp(color.g + colorShift.g + brightness, 0.0, 1.0),
				clamp(color.b + colorShift.b + brightness, 0.0, 1.0),
				1.0);
	
	gl_FragColor = vec4(texture2D(s_contrastMap, vec2(color.r, 0)).r, 
				texture2D(s_contrastMap, vec2(color.g, 0)).g, 
				texture2D(s_contrastMap, vec2(color.b, 0)).b, 
				1.0);
				
	// Vignette effect
	float d = distance(v_texCoord, vec2(0.5,0.5));
	// if d > outer, then 0 -> black 
	// if d < inner, then 1 -> actual color
	// range of (outer - inner) is where the vignette occurs
	gl_FragColor *= smoothstep(outerVignetteDistance, innerVignetteDistance, d);
}	

