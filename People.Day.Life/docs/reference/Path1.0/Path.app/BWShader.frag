precision mediump float;
varying vec2 v_texCoord;
uniform sampler2D s_imageMap;
uniform sampler2D s_contrastMap;
uniform float outerVignetteDistance;
uniform float innerVignetteDistance;
const vec3 lumcoeff = vec3(0.299, 0.587, 0.114);

void main()
{
  	vec4 baseColor = texture2D(s_imageMap, v_texCoord);
	
	float d = distance(v_texCoord, vec2(0.5,0.5));
	// if d > outer, then 0 -> black 
	// if d < inner, then 1 -> actual color
	// range of (outer - inner) is where the vignette occurs
	baseColor *= smoothstep(outerVignetteDistance, innerVignetteDistance, d);
	
	float luminance = dot(baseColor.rgb, lumcoeff);
	float contrastAdjustedLuminance = texture2D(s_contrastMap, vec2(luminance, 0.0)).r;
	gl_FragColor = vec4(contrastAdjustedLuminance, contrastAdjustedLuminance, contrastAdjustedLuminance, 1.0);
}