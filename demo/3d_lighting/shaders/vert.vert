#version 450

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec4 ambientLightColor;
    vec3 lightPosition;
    vec4 lightColor;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 inNormal; // NEW: Vertex normal from C++

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 fragWorldPos;
layout(location = 3) out vec3 fragNormal; // NEW: Pass to fragment shader

layout(push_constant) uniform MeshData {
    mat4 modelMatrix;
} push;

void main() {
    vec4 worldPosition = push.modelMatrix * vec4(inPosition, 1.0);
    gl_Position = ubo.proj * ubo.view * worldPosition;
    
    fragColor = inColor;
    fragTexCoord = inTexCoord;
    fragWorldPos = vec3(worldPosition);
    
    // NEW: Transform the normal to world space
    // Note: If you use non-uniform scaling (e.g., squishing the model on one axis), 
    // you must use: mat3(transpose(inverse(push.modelMatrix))) * inNormal
    fragNormal = mat3(push.modelMatrix) * inNormal; 
}