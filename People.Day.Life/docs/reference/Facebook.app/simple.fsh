//
//  simple.fsh
//  Snap
//
//  Created by John Pottebaum on 11/10/11.
//  Copyright (c) 2011 Facebook. All rights reserved.
//

precision mediump float;

varying vec2 texCoord;
uniform sampler2D srcImg;

void main()
{
  float s = texCoord.x;
  float t = texCoord.y;
  vec4 srcColor = texture2D(srcImg, texCoord);

//  gl_FragColor = vec4(0  ,s ,t, 1.0);
  gl_FragColor = srcColor.grba;

}
