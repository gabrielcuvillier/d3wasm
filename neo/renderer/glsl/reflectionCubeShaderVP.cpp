/*
 * This file is part of the D3wasm project (http://www.continuation-labs.com/projects/d3wasm)
 * Copyright (c) 2019 Gabriel Cuvillier.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "glsl_shaders.h"

const char * const reflectionCubeShaderVP = R"(
#version 100
precision highp float;
  
// In
attribute vec4 attr_Vertex;
attribute mediump vec4 attr_Color;
attribute vec3 attr_TexCoord;
  
// Uniforms
uniform mat4 u_modelViewProjectionMatrix;
uniform mat4 u_modelViewMatrix;
uniform mat4 u_textureMatrix;
uniform mediump float u_colorAdd;
uniform mediump float u_colorModulate;
  
// Out
// gl_Position
varying vec3 var_TexCoord;
varying mediump vec4 var_Color;
  
void main(void)
{
  var_TexCoord = (u_textureMatrix * reflect( normalize( u_modelViewMatrix * attr_Vertex ),
                                            // This suppose the modelView matrix is orthogonal
                                            // Otherwise, we should use the inverse transpose
                                            u_modelViewMatrix * vec4(attr_TexCoord,0.0) )).xyz ;

  if (u_colorModulate == 0.0) {
    var_Color = vec4(u_colorAdd);
  } else {
    var_Color = (attr_Color * u_colorModulate) + vec4(u_colorAdd);
  }
    
  gl_Position = u_modelViewProjectionMatrix * attr_Vertex;
}
)";
