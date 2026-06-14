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

const char * const interactionShaderFP = R"(
#version 100
precision highp float;
  
// In
varying vec2 var_TexDiffuse;
varying vec2 var_TexNormal;
varying vec2 var_TexSpecular;
varying vec4 var_TexLight;
varying mediump vec4 var_Color;
varying vec3 var_L;
varying vec3 var_H;
  
// Uniforms
uniform mediump vec4 u_diffuseColor;
uniform mediump vec4 u_specularColor;
//uniform float u_specularExponent;   // Not used
uniform sampler2D u_fragmentMap0;     // u_bumpTexture
uniform sampler2D u_fragmentMap1;     // u_lightFalloffTexture
uniform sampler2D u_fragmentMap2;     // u_lightProjectionTexture
uniform sampler2D u_fragmentMap3;     // u_diffuseTexture
uniform sampler2D u_fragmentMap4;     // u_specularTexture
  
// Out
// gl_FragCoord
  
void main(void)
{
  vec3 L = normalize(var_L);
  vec3 H = normalize(var_H);
  vec3 N = 2.0 * texture2D(u_fragmentMap0, var_TexNormal.st).agb - 1.0;
  
  float NdotL = clamp(dot(N, L), 0.0, 1.0);
  float NdotH = clamp(dot(N, H), 0.0, 1.0);
  
  vec3 lightProjection = texture2DProj(u_fragmentMap2, var_TexLight.xyw).rgb;
  vec3 lightFalloff = texture2D(u_fragmentMap1, vec2(var_TexLight.z, 0.5)).rgb;
  vec3 diffuseColor = texture2D(u_fragmentMap3, var_TexDiffuse).rgb * u_diffuseColor.rgb;
  vec3 specularColor = 2.0 * texture2D(u_fragmentMap4, var_TexSpecular).rgb * u_specularColor.rgb;
  
  float specularFalloff = pow(NdotH, 12.0); // Hardcoded to try to match with original D3 look
  
  vec3 color;
  color = diffuseColor;
  color += specularFalloff * specularColor;
  color *= NdotL * lightProjection;
  color *= lightFalloff;
  
  gl_FragColor = vec4(color, 1.0) * var_Color;
}
)";
