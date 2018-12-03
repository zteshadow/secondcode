precision mediump float;
varying vec2 v_texCoord;
uniform sampler2D s_imageMap;
const float textureWidth = 480.0; 
const float textureHeight = 360.0; 
const float blockWidth = 6.0;
const float blockHeight = 6.0;
const float shadesPerChannel = 10.0;
const float dcolor = 1.0 / shadesPerChannel;
const float dcolor2 = dcolor / 2.0;
const float dx = blockWidth * (1.0 / textureWidth);
const float dy = blockHeight * (1.0 / textureHeight);

void main() 
{
	vec3 tc = vec3(1.0, 0.0, 0.0);

	vec2 coord = vec2(dx * floor(v_texCoord.x / dx),
				      dy * floor(v_texCoord.y / dy));
	vec4 color = texture2D(s_imageMap, coord);
	gl_FragColor = vec4(dcolor * floor((dcolor2 + color.r) / dcolor),
						dcolor * floor((dcolor2 + color.g) / dcolor),
						dcolor * floor((dcolor2 + color.b) / dcolor), 1.0);	
}
