precision mediump float;
varying vec2 v_texCoord;
uniform sampler2D s_imageMap;

void main()
{
	gl_FragColor = texture2D( s_imageMap, v_texCoord );
}