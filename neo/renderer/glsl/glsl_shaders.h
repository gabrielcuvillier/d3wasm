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

#ifndef D3WASM_GLSL_SHADERS_H
#define D3WASM_GLSL_SHADERS_H

// Main Light Interaction
  // gouraud
extern const char* const interactionShaderVP;
extern const char* const interactionShaderFP;
  // phong
extern const char* const interactionPhongShaderVP;
extern const char* const interactionPhongShaderFP;
// Fog
extern const char* const fogShaderVP;
extern const char* const blendLightShaderVP;
extern const char* const fogShaderFP;
// Depth Buffer
  // no clip planes
extern const char* const zfillShaderVP;
extern const char* const zfillShaderFP;
  // clip planes
extern const char* const zfillClipShaderVP;
extern const char* const zfillClipShaderFP;
// Ambient Surfaces
  // diffuse mapping (default diffuse surfaces)
extern const char* const diffuseMapShaderVP;
extern const char* const diffuseMapShaderFP;
  // cube mapping (skybox/wobblesky, diffusecube, reflection)
extern const char* const diffuseCubeShaderVP;
extern const char* const skyboxCubeShaderVP;
extern const char* const reflectionCubeShaderVP;
extern const char* const cubeMapShaderFP;
// Shadows
extern const char* const stencilShadowShaderVP;
extern const char* const stencilShadowShaderFP;
// HeatHaze
extern const char* const heatHazeShaderVP;
extern const char* const heatHazeShaderFP;
// HeatHazeWithMask
extern const char* const heatHazeWithMaskShaderVP;
extern const char* const heatHazeWithMaskShaderFP;
// HeatHazeWithMaskAndVertex
extern const char* const heatHazeWithMaskAndVertexShaderVP;
extern const char* const heatHazeWithMaskAndVertexShaderFP;
// ColorProcess
extern const char* const colorProcessShaderVP;
extern const char* const colorProcessShaderFP;
// BumpyEnvironment
extern const char* const bumpyEnvironmentShaderVP;
extern const char* const bumpyEnvironmentShaderFP;

// Those shaders are expected to be "public" (found by Materials)
struct shaderProgram_s;
extern struct shaderProgram_s* pheatHazeShader;
extern struct shaderProgram_s* pheatHazeWithMaskShader;
extern struct shaderProgram_s* pheatHazeWithMaskAndVertexShader;
extern struct shaderProgram_s* pcolorProcessShader;

#endif //D3WASM_GLSL_SHADERS_H
