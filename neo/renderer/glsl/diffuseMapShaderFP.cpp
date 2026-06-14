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

const char * const diffuseMapShaderFP = R"(
#version 100
precision highp float;

// In
varying vec2 var_TexCoord;
varying mediump vec4 var_Color;

// Uniforms
uniform sampler2D u_fragmentMap0;
uniform mediump vec4 u_glColor;

// Out
// gl_FragCoord

void main(void)
{
  gl_FragColor = texture2D(u_fragmentMap0, var_TexCoord) * u_glColor * var_Color;
}
)";
