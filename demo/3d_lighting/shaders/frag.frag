#version 450

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec4 ambientLightColor;
    vec3 lightPosition;
    vec4 lightColor;
} ubo;

layout(set = 1, binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragWorldPos;
layout(location = 3) in vec3 fragNormal; // NEW: Receives interpolated normal

layout(location = 0) out vec4 outColor;

void main() {
    // NEW: Normalize the interpolated vector
    vec3 normal = normalize(fragNormal);
    
    vec3 lightDir = normalize(ubo.lightPosition - fragWorldPos);
    
    float diffIntensity = max(dot(normal, lightDir), 0.0);

    vec3 diffuse = diffIntensity * (ubo.lightColor.rgb * ubo.lightColor.a);
    
    vec3 ambient = ubo.ambientLightColor.rgb * ubo.ambientLightColor.a;
    
    vec4 texColor = texture(texSampler, fragTexCoord);
    
    vec3 lighting = ambient + diffuse;
    vec3 finalColor = lighting * texColor.rgb;
    
    outColor = vec4(finalColor, texColor.a);
}