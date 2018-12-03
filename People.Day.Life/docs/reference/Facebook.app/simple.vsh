//
//  simple.vsh
//  Snap
//
//  Created by John Pottebaum on 11/10/11.
//  Copyright (c) 2011 Facebook. All rights reserved.
//


attribute vec4 vertex;
attribute vec2 _texCoordAttrib;
varying vec2 texCoord;

void main() {
  texCoord = _texCoordAttrib;
	gl_Position = vertex;
}
