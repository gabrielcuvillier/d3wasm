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

const char * const interactionShaderVP = R"(
#version 100
precision highp float;
  
// In
attribute vec4 attr_Vertex;
attribute mediump vec4 attr_Color;
attribute vec4 attr_TexCoord;
attribute vec3 attr_Tangent;
attribute vec3 attr_Bitangent;
attribute vec3 attr_Normal;

// Uniforms
uniform mat4 u_modelViewProjectionMatrix;
uniform mat4 u_lightProjection;
uniform mediump float u_colorModulate;
uniform mediump float u_colorAdd;
uniform vec4 u_lightOrigin;
uniform vec4 u_viewOrigin;
uniform vec4 u_bumpMatrixS;
uniform vec4 u_bumpMatrixT;
uniform vec4 u_diffuseMatrixS;
uniform vec4 u_diffuseMatrixT;
uniform vec4 u_specularMatrixS;
uniform vec4 u_specularMatrixT;

// Out
// gl_Position
varying vec2 var_TexDiffuse;
varying vec2 var_TexNormal;
varying vec2 var_TexSpecular;
varying vec4 var_TexLight;
varying mediump vec4 var_Color;
varying vec3 var_L;
varying vec3 var_H;
  
void main(void)
{
  mat3 M = mat3(attr_Tangent, attr_Bitangent, attr_Normal);
  
  var_TexNormal.x = dot(u_bumpMatrixS, attr_TexCoord);
  var_TexNormal.y = dot(u_bumpMatrixT, attr_TexCoord);
  
  var_TexDiffuse.x = dot(u_diffuseMatrixS, attr_TexCoord);
  var_TexDiffuse.y = dot(u_diffuseMatrixT, attr_TexCoord);
  
  var_TexSpecular.x = dot(u_specularMatrixS, attr_TexCoord);
  var_TexSpecular.y = dot(u_specularMatrixT, attr_TexCoord);
  
  var_TexLight.x = dot(u_lightProjection[0], attr_Vertex);
  var_TexLight.y = dot(u_lightProjection[1], attr_Vertex);
  var_TexLight.z = dot(u_lightProjection[2], attr_Vertex);
  var_TexLight.w = dot(u_lightProjection[3], attr_Vertex);
  
  vec3 L = u_lightOrigin.xyz - attr_Vertex.xyz;
  vec3 V = u_viewOrigin.xyz - attr_Vertex.xyz;
  vec3 H = normalize(L) + normalize(V);
  
  var_L = L * M;
  var_H = H * M;

  if (u_colorModulate == 0.0) {
    var_Color = vec4(u_colorAdd);
  } else {
    var_Color = (attr_Color * u_colorModulate) + vec4(u_colorAdd);
  }
  
  gl_Position = u_modelViewProjectionMatrix * attr_Vertex;
}
)";
