/*
 * Copyright (c) 2026 MidnightHammer-code
 * This source code is licensed under the GPL 3.0 license
 * LICENSE file in the root directory of this source tree.
 */

#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

layout(push_constant) uniform MeshData {
    mat4 modelMatrix;
} push;

void main() {
    vec4 worldPosition = push.modelMatrix * vec4(inPosition, 1.0);
    gl_Position = ubo.proj * ubo.view * worldPosition;
    
    fragColor = inColor;
    fragTexCoord = inTexCoord;
}